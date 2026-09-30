/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*
 * grib_grib2mars_replay
 *
 * Engine used by the tests/grib_grib2mars_replay.sh test.
 *
 * It reads a JSON-lines stream on stdin (the test script decompresses
 * data/grib2mars_replay.zst on the fly with `zstd -dc | grib_grib2mars_replay`) and for
 * every line
 *
 *   1. clones a template handle created once from the GRIB2 sample file,
 *   2. replays the "set" and "set_missing" operations of the JSON line
 *      (make_from_sample, clone and the "values" set are ignored - see below),
 *   3. builds two flat, space-separated strings and compares them:
 *
 *        expected = origin class type stream expver date time step param level packing
 *        got      = centre marsClass marsType marsStream expver dataDate
 *                   dataTime step paramId (level|-) packingType
 *
 *      Level is expressed in Pa in the reference and in hPa in ecCodes for
 *      isobaric-in-hPa levels, so we scale the expected value by 100.
 *      Packing is written by the reference as "ccsds", "complex", ... whereas
 *      ecCodes' packingType has a "grid_" or "spectral_" prefix, so we prefix
 *      the expected value based on gridType == "sh".
 *
 * This is the approach implemented by mirco's grib_check.cpp:
 *   /home/joobog/git/playground/people/mirco/test_data/grib_check.cpp
 * We keep the same set of comparison keys (no MARS namespace iteration, no
 * final re-parse, no intermediate clones) so that the test runs in a few
 * tens of seconds on a Debug libeccodes.
 *
 * Usage:
 *   zstd -dc data/grib2mars_replay.zst | grib_grib2mars_replay <GRIB2.tmpl>
 */

#include <cctype>
#include <cerrno>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <string>
#include <vector>

#include "eccodes.h"

// ---------------------------------------------------------------------------
// Minimal JSON parser (no external dependency)
// ---------------------------------------------------------------------------

struct JsonValue
{
    enum Type { Null, Bool, Int, Double, String, Array, Object };

    Type type = Null;
    bool bval  = false;
    long long ival = 0;
    double dval = 0;
    std::string sval;
    std::vector<JsonValue> items;
    std::vector<std::string> keys;
    std::vector<JsonValue> vals;

    bool isNumber() const { return type == Int || type == Double; }
    double asDouble() const { return type == Int ? static_cast<double>(ival) : dval; }
    long long asInt() const { return type == Int ? ival : static_cast<long long>(dval); }

    const JsonValue* member(const char* name) const
    {
        if (type != Object) return nullptr;
        for (size_t i = 0; i < keys.size(); ++i)
            if (keys[i] == name) return &vals[i];
        return nullptr;
    }
};

class JsonParser
{
public:
    JsonParser(const char* p, size_t len) : p_(p), end_(p + len) {}

    bool parse(JsonValue& out)
    {
        skipWs();
        if (!parseValue(out)) return false;
        skipWs();
        return true;
    }

    const std::string& error() const { return error_; }

private:
    const char* p_;
    const char* end_;
    std::string error_;

    bool fail(const char* msg) { if (error_.empty()) error_ = msg; return false; }

    void skipWs()
    {
        while (p_ < end_ && (*p_ == ' ' || *p_ == '\t' || *p_ == '\n' || *p_ == '\r')) ++p_;
    }

    bool parseValue(JsonValue& v)
    {
        if (p_ >= end_) return fail("unexpected end of input");
        switch (*p_) {
            case '{': return parseObject(v);
            case '[': return parseArray(v);
            case '"': v.type = JsonValue::String; return parseString(v.sval);
            case 't':
                if (end_ - p_ < 4 || strncmp(p_, "true", 4) != 0) return fail("bad literal");
                p_ += 4; v.type = JsonValue::Bool; v.bval = true; return true;
            case 'f':
                if (end_ - p_ < 5 || strncmp(p_, "false", 5) != 0) return fail("bad literal");
                p_ += 5; v.type = JsonValue::Bool; v.bval = false; return true;
            case 'n':
                if (end_ - p_ < 4 || strncmp(p_, "null", 4) != 0) return fail("bad literal");
                p_ += 4; v.type = JsonValue::Null; return true;
            default: return parseNumber(v);
        }
    }

    bool parseObject(JsonValue& v)
    {
        v.type = JsonValue::Object;
        ++p_;
        skipWs();
        if (p_ < end_ && *p_ == '}') { ++p_; return true; }
        for (;;) {
            skipWs();
            std::string key;
            if (p_ >= end_ || *p_ != '"') return fail("expected object key");
            if (!parseString(key)) return false;
            skipWs();
            if (p_ >= end_ || *p_ != ':') return fail("expected ':'");
            ++p_;
            skipWs();
            v.keys.push_back(key);
            v.vals.push_back(JsonValue());
            if (!parseValue(v.vals.back())) return false;
            skipWs();
            if (p_ < end_ && *p_ == ',') { ++p_; continue; }
            if (p_ < end_ && *p_ == '}') { ++p_; return true; }
            return fail("expected ',' or '}'");
        }
    }

    bool parseArray(JsonValue& v)
    {
        v.type = JsonValue::Array;
        ++p_;
        skipWs();
        if (p_ < end_ && *p_ == ']') { ++p_; return true; }
        for (;;) {
            skipWs();
            v.items.push_back(JsonValue());
            if (!parseValue(v.items.back())) return false;
            skipWs();
            if (p_ < end_ && *p_ == ',') { ++p_; continue; }
            if (p_ < end_ && *p_ == ']') { ++p_; return true; }
            return fail("expected ',' or ']'");
        }
    }

    bool parseString(std::string& out)
    {
        out.clear();
        ++p_;
        while (p_ < end_) {
            char c = *p_++;
            if (c == '"') return true;
            if (c != '\\') { out.push_back(c); continue; }
            if (p_ >= end_) return fail("bad escape");
            char e = *p_++;
            switch (e) {
                case '"':  out.push_back('"');  break;
                case '\\': out.push_back('\\'); break;
                case '/':  out.push_back('/');  break;
                case 'b':  out.push_back('\b'); break;
                case 'f':  out.push_back('\f'); break;
                case 'n':  out.push_back('\n'); break;
                case 'r':  out.push_back('\r'); break;
                case 't':  out.push_back('\t'); break;
                case 'u': {
                    if (end_ - p_ < 4) return fail("bad \\u escape");
                    unsigned code = 0;
                    for (int i = 0; i < 4; ++i) {
                        char h = *p_++;
                        code <<= 4;
                        if (h >= '0' && h <= '9')      code |= static_cast<unsigned>(h - '0');
                        else if (h >= 'a' && h <= 'f') code |= static_cast<unsigned>(h - 'a' + 10);
                        else if (h >= 'A' && h <= 'F') code |= static_cast<unsigned>(h - 'A' + 10);
                        else return fail("bad \\u escape");
                    }
                    if (code < 0x80)      { out.push_back(static_cast<char>(code)); }
                    else if (code < 0x800) {
                        out.push_back(static_cast<char>(0xC0 | (code >> 6)));
                        out.push_back(static_cast<char>(0x80 | (code & 0x3F)));
                    } else {
                        out.push_back(static_cast<char>(0xE0 | (code >> 12)));
                        out.push_back(static_cast<char>(0x80 | ((code >> 6) & 0x3F)));
                        out.push_back(static_cast<char>(0x80 | (code & 0x3F)));
                    }
                    break;
                }
                default: return fail("bad escape");
            }
        }
        return fail("unterminated string");
    }

    bool parseNumber(JsonValue& v)
    {
        const char* start = p_;
        bool isInt = true;
        if (p_ < end_ && (*p_ == '-' || *p_ == '+')) ++p_;
        while (p_ < end_) {
            char c = *p_;
            if (isdigit(static_cast<unsigned char>(c))) { ++p_; continue; }
            if (c == '.' || c == 'e' || c == 'E' || c == '+' || c == '-') { isInt = false; ++p_; continue; }
            break;
        }
        if (p_ == start) return fail("expected a number");
        std::string tok(start, static_cast<size_t>(p_ - start));
        if (isInt) {
            char* endp = nullptr;
            errno = 0;
            long long n = strtoll(tok.c_str(), &endp, 10);
            if (errno == 0 && endp && *endp == '\0') {
                v.type = JsonValue::Int;
                v.ival = n;
                return true;
            }
        }
        v.type = JsonValue::Double;
        v.dval = strtod(tok.c_str(), nullptr);
        return true;
    }
};

// ---------------------------------------------------------------------------
// Encoding
// ---------------------------------------------------------------------------

/*
 * Apply one "set" operation to the handle.
 * Returns a CODES_* error code (CODES_SUCCESS on success).
 * "values" is deliberately ignored: the reference stream only records the
 * average of the data payload, not the payload itself.
 */
static int applySet(codes_handle* h, const JsonValue& args)
{
    const JsonValue* keyv = args.member("key");
    const JsonValue* dtv  = args.member("datatype");
    const JsonValue* valv = args.member("value");
    if (!keyv || keyv->type != JsonValue::String) return CODES_INVALID_ARGUMENT;
    const std::string& key = keyv->sval;

    /* Skip the data payload: the reference stores only its average */
    if (key == "values") return CODES_SUCCESS;

    std::string dtype = "integer";
    long rank = 0;
    if (dtv) {
        const JsonValue* t = dtv->member("type");
        const JsonValue* r = dtv->member("rank");
        if (t && t->type == JsonValue::String) dtype = t->sval;
        if (r && r->isNumber()) rank = static_cast<long>(r->asInt());
    }
    if (!valv) return CODES_INVALID_ARGUMENT;

    if (rank == 1) {
        if (valv->type != JsonValue::Array) return CODES_SUCCESS;  /* placeholder */
        size_t n = valv->items.size();
        if (dtype == "double") {
            std::vector<double> v(n);
            for (size_t i = 0; i < n; ++i) v[i] = valv->items[i].asDouble();
            return codes_set_double_array(h, key.c_str(), n ? v.data() : nullptr, n);
        }
        std::vector<long> v(n);
        for (size_t i = 0; i < n; ++i) v[i] = static_cast<long>(valv->items[i].asInt());
        return codes_set_long_array(h, key.c_str(), n ? v.data() : nullptr, n);
    }

    if (dtype == "string") {
        std::string s = valv->type == JsonValue::String ? valv->sval : std::string();
        size_t len = s.size();
        return codes_set_string(h, key.c_str(), s.c_str(), &len);
    }
    if (dtype == "double") {
        return codes_set_double(h, key.c_str(), valv->asDouble());
    }
    return codes_set_long(h, key.c_str(), static_cast<long>(valv->asInt()));
}

static int applySetMissing(codes_handle* h, const JsonValue& args)
{
    const JsonValue* keyv = args.member("key");
    if (!keyv || keyv->type != JsonValue::String) return CODES_INVALID_ARGUMENT;
    return codes_set_missing(h, keyv->sval.c_str());
}

// ---------------------------------------------------------------------------
// Extract / expected
// ---------------------------------------------------------------------------

/* Read a string key off the handle, empty string on error */
static std::string getStr(codes_handle* h, const char* key)
{
    char buf[256];
    size_t n = sizeof(buf);
    if (codes_get_string(h, key, buf, &n) == CODES_SUCCESS) return std::string(buf);
    return std::string();
}

static long getLong(codes_handle* h, const char* key)
{
    long v = 0;
    if (codes_get_long(h, key, &v) == CODES_SUCCESS) return v;
    return 0;
}

static void appendLong(std::string& out, long v)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "%ld", v);
    out += buf;
}

/*
 * Extract the fields we compare from an encoded handle:
 *   centre marsClass marsType marsStream expver dataDate dataTime step
 *   paramId (level|-) packingType
 * Also return gridType so the caller can pick the right packing prefix.
 */
struct Got {
    std::string flat;
    std::string gridType;
};

static Got extractGot(codes_handle* h, bool hasLevelist)
{
    Got g;
    g.flat.reserve(96);

    g.flat += getStr(h, "centre");         g.flat += ' ';
    g.flat += getStr(h, "marsClass");      g.flat += ' ';
    g.flat += getStr(h, "marsType");       g.flat += ' ';
    g.flat += getStr(h, "marsStream");     g.flat += ' ';
    g.flat += getStr(h, "expver");         g.flat += ' ';
    appendLong(g.flat, getLong(h, "dataDate")); g.flat += ' ';
    appendLong(g.flat, getLong(h, "dataTime")); g.flat += ' ';
    appendLong(g.flat, getLong(h, "step"));     g.flat += ' ';
    appendLong(g.flat, getLong(h, "paramId"));  g.flat += ' ';
    if (hasLevelist) appendLong(g.flat, getLong(h, "level"));
    else             g.flat += '-';
    g.flat += ' ';
    g.flat += getStr(h, "packingType");

    g.gridType = getStr(h, "gridType");
    return g;
}

static bool hasLevelist(const JsonValue& mars)
{
    const JsonValue* v = mars.member("levelist");
    return v != nullptr && v->type != JsonValue::Null;
}

/*
 * Build the expected string from the "mars" object of the JSON line.
 * The reference stores 'levelist' in Pa; ecCodes' 'level' for
 * isobaricInhPa levels is in hPa, so we divide by 100 for levtype "pl".
 * 'packing' has no prefix in the reference; ecCodes' packingType prefixes
 * with "grid_" (regular grid) or "spectral_" (spherical harmonics).
 */
static std::string buildExpected(const JsonValue& mars, bool gridIsSh)
{
    std::string s;
    s.reserve(96);

    auto getSv = [&](const char* key) -> std::string {
        const JsonValue* v = mars.member(key);
        return (v && v->type == JsonValue::String) ? v->sval : std::string();
    };
    auto appendInt = [&](const char* key) {
        const JsonValue* v = mars.member(key);
        if (v && v->isNumber()) appendLong(s, static_cast<long>(v->asInt()));
    };

    s += getSv("origin");   s += ' ';
    s += getSv("class");    s += ' ';
    s += getSv("type");     s += ' ';
    s += getSv("stream");   s += ' ';

    /* expver may be a string ("2250") or an integer, depending on the dataset */
    {
        const JsonValue* v = mars.member("expver");
        if (v) {
            if (v->type == JsonValue::String)      s += v->sval;
            else if (v->isNumber())                appendLong(s, static_cast<long>(v->asInt()));
        }
    }
    s += ' ';

    appendInt("date");      s += ' ';
    appendInt("time");      s += ' ';
    appendInt("step");      s += ' ';
    appendInt("param");     s += ' ';

    {
        const JsonValue* lv = mars.member("levelist");
        if (lv && lv->isNumber()) {
            long v = static_cast<long>(lv->asInt());
            std::string lt = getSv("levtype");
            if (lt == "pl") v /= 100;
            appendLong(s, v);
        }
        else {
            s += '-';
        }
    }
    s += ' ';

    s += gridIsSh ? "spectral" : "grid";
    s += '_';
    s += getSv("packing");

    return s;
}

// ---------------------------------------------------------------------------
// Per line
// ---------------------------------------------------------------------------

struct Stats {
    long processed = 0;
    long mismatches = 0;
    long parseErrors = 0;
    long setErrors = 0;
};

/*
 * Process one JSON line.
 * Any mismatch, parse error or set error is a hard failure: we log a
 * self-contained reproduction block to 'logFp' (if any) and count it in
 * 'stats'. 'lineNo' is the 1-based index of the JSON line in the input.
 */
static void processLine(const std::string& line, long lineNo, codes_handle* sample,
                        Stats& stats, std::FILE* logFp, long maxLogged,
                        const std::string& inputPath, const std::string& samplePath)
{
    JsonValue doc;
    JsonParser parser(line.data(), line.size());
    if (!parser.parse(doc)) {
        ++stats.parseErrors;
        if (logFp && (stats.mismatches + stats.parseErrors + stats.setErrors) <= maxLogged) {
            fprintf(logFp,
                    "=== FAIL line %ld: JSON parse error: %s\n"
                    "    Reproduce:\n"
                    "      zstd -dc --long=31 %s | sed -n '%ldp'\n"
                    "    JSON:\n%s\n\n",
                    lineNo, parser.error().c_str(),
                    inputPath.c_str(), lineNo, line.c_str());
        }
        return;
    }

    const JsonValue* mars = doc.member("mars");
    const JsonValue* out  = doc.member("out");
    const JsonValue* ops  = out ? out->member("operations") : nullptr;
    if (!mars || mars->type != JsonValue::Object || !ops || ops->type != JsonValue::Array) {
        ++stats.parseErrors;
        if (logFp && (stats.mismatches + stats.parseErrors + stats.setErrors) <= maxLogged) {
            fprintf(logFp,
                    "=== FAIL line %ld: missing 'mars' or 'out.operations'\n"
                    "    Reproduce:\n"
                    "      zstd -dc --long=31 %s | sed -n '%ldp'\n"
                    "    JSON:\n%s\n\n",
                    lineNo, inputPath.c_str(), lineNo, line.c_str());
        }
        return;
    }

    codes_handle* h = codes_handle_clone(sample);
    if (!h) { ++stats.setErrors; return; }

    bool setOk = true;
    int setRc = CODES_SUCCESS;
    size_t failedOp = 0;
    std::string failedKey;
    for (size_t i = 0; i < ops->items.size(); ++i) {
        const JsonValue& op = ops->items[i];
        if (op.type != JsonValue::Object || op.keys.empty()) continue;
        const std::string& name = op.keys[0];
        int rc = CODES_SUCCESS;
        if (name == "set")               rc = applySet(h, op.vals[0]);
        else if (name == "set_missing")  rc = applySetMissing(h, op.vals[0]);
        /* make_from_sample and clone are ignored */
        if (rc != CODES_SUCCESS) {
            setOk = false; setRc = rc; failedOp = i;
            const JsonValue* kv = op.vals[0].member("key");
            if (kv && kv->type == JsonValue::String) failedKey = kv->sval;
            break;
        }
    }

    if (!setOk) {
        ++stats.setErrors;
        if (logFp && (stats.mismatches + stats.parseErrors + stats.setErrors) <= maxLogged) {
            fprintf(logFp,
                    "=== FAIL line %ld: operation %zu (set %s) failed: %s\n"
                    "    Reproduce:\n"
                    "      # from the input file:\n"
                    "      zstd -dc --long=31 %s | sed -n '%ldp' | \\\n"
                    "        %s %s\n"
                    "      # or directly from the JSON below:\n"
                    "      %s %s <<'EOF'\n%s\nEOF\n\n",
                    lineNo, failedOp, failedKey.c_str(), codes_get_error_message(setRc),
                    inputPath.c_str(), lineNo,
                    "./grib_grib2mars_replay", samplePath.c_str(),
                    "./grib_grib2mars_replay", samplePath.c_str(),
                    line.c_str());
        }
        codes_handle_delete(h);
        return;
    }

    Got got = extractGot(h, hasLevelist(*mars));
    std::string expected = buildExpected(*mars, got.gridType == "sh");

    ++stats.processed;
    if (got.flat != expected) {
        ++stats.mismatches;
        if (logFp && (stats.mismatches + stats.parseErrors + stats.setErrors) <= maxLogged) {
            fprintf(logFp,
                    "=== FAIL line %ld: MARS keys mismatch\n"
                    "    expected: [%s]\n"
                    "    got     : [%s]\n"
                    "    Reproduce:\n"
                    "      # from the input file:\n"
                    "      zstd -dc --long=31 %s | sed -n '%ldp' | \\\n"
                    "        %s %s\n"
                    "      # or directly from the JSON below:\n"
                    "      %s %s <<'EOF'\n%s\nEOF\n\n",
                    lineNo, expected.c_str(), got.flat.c_str(),
                    inputPath.c_str(), lineNo,
                    "./grib_grib2mars_replay", samplePath.c_str(),
                    "./grib_grib2mars_replay", samplePath.c_str(),
                    line.c_str());
        }
    }

    codes_handle_delete(h);
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------

static void usage(const char* prog)
{
    fprintf(stderr,
            "Usage: %s [--log FILE] [--input NAME] [-e MAX] <GRIB2.tmpl>\n"
            "\n"
            "  Reads JSON-lines on stdin (typically piped from `zstd -dc`).\n"
            "  Any mismatch, parse error or set error is a hard failure: a\n"
            "  self-contained reproduction block is written to the log file\n"
            "  and the program exits non-zero.\n"
            "\n"
            "Options:\n"
            "  --log FILE     Write failure reports here. Default: stderr\n"
            "  --input NAME   Name to reference in the reproduction commands.\n"
            "                 Default: <stdin>\n"
            "  -e N           Log at most N failures. Default: 50\n",
            prog);
}

int main(int argc, char* argv[])
{
    long maxLogged = 50;
    std::string logPath;
    std::string inputName = "<stdin>";
    int i = 1;
    for (; i < argc; ++i) {
        std::string a = argv[i];
        if      (a == "-e"      && i + 1 < argc) maxLogged  = atol(argv[++i]);
        else if (a == "--log"   && i + 1 < argc) logPath    = argv[++i];
        else if (a == "--input" && i + 1 < argc) inputName  = argv[++i];
        else if (a == "-h" || a == "--help")     { usage(argv[0]); return 0; }
        else break;
    }
    if (argc - i != 1) { usage(argv[0]); return 1; }
    std::string samplePath = argv[i];

    printf("grib_grib2mars_replay: sample=%s input=%s\n",
           samplePath.c_str(), inputName.c_str());
    fflush(stdout);

    /* Open the GRIB2 sample once; every message clones it */
    FILE* fp = fopen(samplePath.c_str(), "rb");
    if (!fp) {
        fprintf(stderr, "ERROR: cannot open sample '%s': %s\n", samplePath.c_str(), strerror(errno));
        return 1;
    }
    int err = 0;
    codes_handle* sample = codes_handle_new_from_file(nullptr, fp, PRODUCT_GRIB, &err);
    fclose(fp);
    if (!sample || err != CODES_SUCCESS) {
        fprintf(stderr, "ERROR: cannot create a handle from '%s': %s\n",
                samplePath.c_str(), codes_get_error_message(err));
        if (sample) codes_handle_delete(sample);
        return 1;
    }

    FILE* logFp = stderr;
    if (!logPath.empty()) {
        logFp = fopen(logPath.c_str(), "w");
        if (!logFp) {
            fprintf(stderr, "ERROR: cannot open log '%s': %s\n", logPath.c_str(), strerror(errno));
            codes_handle_delete(sample);
            return 1;
        }
    }

    Stats stats;
    std::string line;
    line.reserve(4096);
    long lineNo = 0;
    int c;
    while ((c = fgetc(stdin)) != EOF) {
        if (c == '\n') {
            ++lineNo;
            if (!line.empty()) {
                if (line.back() == '\r') line.pop_back();
                processLine(line, lineNo, sample, stats, logFp, maxLogged, inputName, samplePath);
                line.clear();
            }
        }
        else {
            line.push_back(static_cast<char>(c));
        }
    }
    if (!line.empty()) {
        ++lineNo;
        processLine(line, lineNo, sample, stats, logFp, maxLogged, inputName, samplePath);
    }

    printf("TOTAL messages=%ld mismatching=%ld parse_errors=%ld set_errors=%ld\n",
           stats.processed, stats.mismatches, stats.parseErrors, stats.setErrors);

    long failures = stats.mismatches + stats.parseErrors + stats.setErrors;
    if (failures > 0 && logFp) {
        fprintf(logFp, "=== SUMMARY: %ld failure(s) out of %ld messages\n",
                failures, stats.processed);
        if (failures > maxLogged)
            fprintf(logFp, "    (only the first %ld failure(s) were reported)\n", maxLogged);
    }
    if (logFp && logFp != stderr) fclose(logFp);

    codes_handle_delete(sample);
    return failures > 0 ? 1 : 0;
}
