#include "AccessorInitData.h"
#include "AccessorUtils/GribType.h"
#include "grib_api_internal.h"

namespace eccodes::accessor {

AccessorInitData makeInitData(grib_section* section,  long len, grib_arguments* args)
{
    AccessorInitData initData{len};

    grib_arguments* next_arg = args;

    while(next_arg)
    {
        if(strcmp(next_arg->expression->cclass->name, "accessor")==0)
        {
            initData.args.emplace_back(next_arg->expression->cclass->name,
                                  grib_expression_get_name(next_arg->expression));
        }
        else switch(grib_expression_native_type(section->h, next_arg->expression))
        {
            case GRIB_TYPE_LONG: 
            {
                long result{};
                grib_expression_evaluate_long(section->h, next_arg->expression, &result);
                initData.args.emplace_back(toString(GribType::LONG), result);
                break;
            }
            case GRIB_TYPE_DOUBLE:
            {
                double result{};
                grib_expression_evaluate_double(section->h, next_arg->expression, &result);
                initData.args.emplace_back(toString(GribType::DOUBLE), result);
                break;
            }
            case GRIB_TYPE_STRING:
            {
                const int BUFFER_SIZE{128};
                char buf[BUFFER_SIZE] = {0};
                size_t bufSize{BUFFER_SIZE};
                int err{0};
                grib_expression_evaluate_string(section->h, next_arg->expression, buf, &bufSize, &err);
                initData.args.emplace_back(toString(GribType::STRING), buf);
                break;
            }
            default: 
            {
                initData.args.emplace_back(toString(GribType::UNDEFINED), 0);
                break;
            }
        }

        next_arg = next_arg->next;
    }

    return initData;
}

}
