#
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.
#

list( APPEND eccodes_src_files
    step_unit.cc
    step.cc
    step_utilities.cc
    grib_api.h
    eccodes.h

    action/Action.cc
    action/Alias.cc
    action/Gen.cc
    action/If.cc
    action/Switch.cc
    action/Section.cc
    action/List.cc
    action/While.cc
    action/Put.cc
    action/Meta.cc
    action/Remove.cc
    action/Rename.cc
    action/Assert.cc
    action/Template.cc
    action/Trigger.cc
    action/When.cc
    action/Concept.cc
    action/HashArray.cc
    action/Set.cc
    action/SetSarray.cc
    action/SetDarray.cc
    action/Noop.cc
    action/Write.cc
    action/Print.cc
    action/Close.cc
    action/Variable.cc
    action/Modify.cc
    action/TransientDarray.cc
    action/SetMissing.cc

    eccodes.cc
    grib_concept.cc
    grib_hash_array.cc
    grib_bufr_descriptor.cc
    grib_bufr_descriptors_array.cc
    grib_darray.cc
    grib_sarray.cc
    grib_oarray.cc
    grib_vdarray.cc
    grib_vsarray.cc
    grib_iarray.cc
    grib_viarray.cc
    codes_memfs.cc

    accessor/grib_accessors_list.cc

    accessor/Accessor.cc
    accessor/G1FcPeriod.cc
    accessor/G1EndOfIntervalMonthly.cc
    accessor/MarsParam.cc
    accessor/Gen.cc
    accessor/Ascii.cc
    accessor/Bit.cc
    accessor/Bitmap.cc
    accessor/Bits.cc
    accessor/BitsPerValue.cc
    accessor/BufrElementsTable.cc
    accessor/BufrDataArray.cc
    accessor/BufrDataElement.cc
    accessor/BufrGroup.cc
    accessor/BufrStringValues.cc
    accessor/PackBufrValues.cc
    accessor/UnpackBufrValues.cc
    accessor/Group.cc
    accessor/NonAlpha.cc
    accessor/G1Bitmap.cc
    accessor/G2Bitmap.cc
    accessor/Concept.cc
    accessor/HashArray.cc
    accessor/DecimalPrecision.cc
    accessor/DivDouble.cc
    accessor/MultDouble.cc
    accessor/BudgDate.cc
    accessor/ValidityDate.cc
    accessor/ValidityTime.cc
    accessor/Bytes.cc
    accessor/CfVarName.cc
    accessor/ChangeAlternativeRowScanning.cc
    accessor/ChangeScanningDirection.cc
    accessor/CheckInternalVersion.cc
    accessor/Codeflag.cc
    accessor/SmartTable.cc
    accessor/SmartTableColumn.cc
    accessor/Codetable.cc
    accessor/CodetableUnits.cc
    accessor/CodetableTitle.cc
    accessor/StepHumanReadable.cc
    accessor/CountFile.cc
    accessor/CountTotal.cc
    accessor/Double.cc
    accessor/Element.cc
    accessor/Evaluate.cc
    accessor/G1Date.cc
    accessor/G1MonthlyDate.cc
    accessor/LibraryVersion.cc
    accessor/When.cc
    accessor/G1VerificationDate.cc
    accessor/G1DayOfTheYearDate.cc
    accessor/G1HalfByteCodeflag.cc
    accessor/G1ForecastMonth.cc
    accessor/G1StepRange.cc
    accessor/G2StepRange.cc
    accessor/DataG22OrderPacking.cc
    accessor/MarsStep.cc
    accessor/MessageCopy.cc
    accessor/MessageIsValid.cc
    accessor/Dictionary.cc
    accessor/LatLonIncrement.cc
    accessor/G2Date.cc
    accessor/G2Level.cc
    accessor/G2EndStep.cc
    accessor/G2LatLon.cc
    accessor/G2Lon.cc
    accessor/GlobalGaussian.cc
    accessor/GaussianGridName.cc
    accessor/Getenv.cc
    accessor/GtsHeader.cc
    accessor/JulianDay.cc
    accessor/JulianDate.cc
    accessor/ClosestDate.cc
    accessor/LatLonValues.cc
    accessor/Latitudes.cc
    accessor/Longitudes.cc
    accessor/OffsetFile.cc
    accessor/Scale.cc
    accessor/FromScaleFactorScaledValue.cc
    accessor/G2BitmapPresent.cc
    accessor/IbmFloat.cc
    accessor/IeeeFloat.cc
    accessor/Constant.cc
    accessor/Iterator.cc
    accessor/Message.cc
    accessor/Nearest.cc
    accessor/Ksec1Expver.cc
    accessor/Label.cc
    accessor/Long.cc
    accessor/Lookup.cc
    accessor/OctahedralGaussian.cc
    accessor/OctetNumber.cc
    accessor/HeadersOnly.cc
    accessor/Padding.cc
    accessor/Pad.cc
    accessor/PadTo.cc
    accessor/PadToEven.cc
    accessor/PadToMultiple.cc
    accessor/SectionPadding.cc
    accessor/PackingType.cc
    accessor/SectionPointer.cc
    accessor/Position.cc
    accessor/Signed.cc
    accessor/SignedBits.cc
    accessor/Section.cc
    accessor/StepInUnits.cc
    accessor/OptimalStepUnits.cc
    accessor/SectionLength.cc
    accessor/G1MessageLength.cc
    accessor/G1Section4Length.cc
    accessor/Size.cc
    accessor/RdbTimeGuessDate.cc
    accessor/ScaleValues.cc
    accessor/OffsetValues.cc
    accessor/Sprintf.cc
    accessor/Round.cc
    accessor/SpectralTruncation.cc
    accessor/Time.cc
    accessor/Transient.cc
    accessor/Trim.cc
    accessor/TransientDarray.cc
    accessor/Values.cc
    accessor/SimplePackingError.cc
    accessor/DataSimplePacking.cc
    accessor/CountMissing.cc
    accessor/DataShPacked.cc
    accessor/DataShUnpacked.cc
    accessor/NumberOfValuesDataRawPacking.cc
    accessor/DataG1SimplePacking.cc
    accessor/DataG1ShSimplePacking.cc
    accessor/DataShSimplePacking.cc
    accessor/DataDummyField.cc
    accessor/Variable.cc
    accessor/SecondOrderBitsPerValue.cc
    accessor/DataG2SimplePacking.cc
    accessor/DataG2SimplePackingWithPreprocessing.cc
    accessor/DataG2ShSimplePacking.cc
    accessor/DataG2ComplexPacking.cc
    accessor/DataG1SecondOrderRowByRowPacking.cc
    accessor/DataG1SecondOrderConstantWidthPacking.cc
    accessor/DataG1SecondOrderGeneralPacking.cc
    accessor/DataG1SecondOrderGeneralExtendedPacking.cc
    accessor/G2Grid.cc
    accessor/UnexpandedDescriptors.cc
    accessor/ExpandedDescriptors.cc
    accessor/BufrdcExpandedDescriptors.cc
    accessor/DataApplyBitmap.cc
    accessor/DataApplyBoustrophedonic.cc
    accessor/DataApplyBoustrophedonicBitmap.cc
    accessor/DataSecondaryBitmap.cc
    accessor/DataG1SecondaryBitmap.cc
    accessor/DataG2SecondaryBitmap.cc
    accessor/DataJpeg2000Packing.cc
    accessor/DataPngPacking.cc
    accessor/DataCcsdsPacking.cc
    accessor/DataRawPacking.cc
    accessor/DataComplexPacking.cc
    accessor/DataG1ComplexPacking.cc
    accessor/GdsIsPresent.cc
    accessor/SelectStepTemplate.cc
    accessor/LocalDefinition.cc
    accessor/G2Eps.cc
    accessor/G2Aerosol.cc
    accessor/G2Chemical.cc
    accessor/G2ConceptDir.cc
    accessor/Mtg2SwitchDefault.cc
    accessor/G2MarsLabeling.cc
    accessor/Md5.cc
    accessor/ProjString.cc
    accessor/NumberOfPoints.cc
    accessor/Suppressed.cc
    accessor/NumberOfPointsGaussian.cc
    accessor/NumberOfValues.cc
    accessor/NumberOfCodedValues.cc
    accessor/G1NumberOfCodedValuesShComplex.cc
    accessor/G1NumberOfCodedValuesShSimple.cc
    accessor/Dirty.cc
    accessor/Statistics.cc
    accessor/StatisticsSpectral.cc
    accessor/Unsigned.cc
    accessor/UnsignedBits.cc
    accessor/Raw.cc
    accessor/BufrSimpleThinning.cc
    accessor/BufrExtractSubsets.cc
    accessor/BufrExtractAreaSubsets.cc
    accessor/BufrExtractDatetimeSubsets.cc
    accessor/Spd.cc
    accessor/Sum.cc
    accessor/ToInteger.cc
    accessor/ToDouble.cc
    accessor/ToString.cc
    accessor/Sexagesimal2Decimal.cc
    accessor/Vector.cc
    accessor/LongVector.cc
    accessor/AbstractVector.cc
    accessor/AbstractLongVector.cc
    accessor/ReferenceValueError.cc
    accessor/Uint16.cc
    accessor/Uint32.cc
    accessor/Uint32LittleEndian.cc
    accessor/Uint64.cc
    accessor/Uint64LittleEndian.cc
    accessor/Uint8.cc
    accessor/Blob.cc
    accessor/DataG2BifourierPacking.cc
    accessor/DataRunLengthPacking.cc
    accessor/GridSpec.cc
    accessor/G2ParamConceptFilename.cc
    accessor/G2Probability.cc
    accessor/G2WavePeriodRange.cc

    grib_jasper_encoding.cc
    grib_openjpeg_encoding.cc
    grib_index.cc
    geo/grib_gaussian_reduced.cc
    grib_loader_from_handle.cc
    grib_bits.cc
    grib_ibmfloat.cc
    grib_ieeefloat.cc
    grib_memory.cc
    grib_buffer.cc

    dumper/Debug.cc
    dumper/Default.cc
    dumper/BufrEncodeC.cc
    dumper/BufrEncodeFilter.cc
    dumper/BufrEncodeFortran.cc
    dumper/BufrEncodePython.cc
    dumper/BufrDecodeC.cc
    dumper/BufrDecodeFilter.cc
    dumper/BufrDecodeFortran.cc
    dumper/BufrDecodePython.cc
    dumper/BufrSimple.cc
    dumper/Json.cc
    dumper/GribEncodeC.cc
    dumper/Wmo.cc

    grib_context.cc
    grib_date.cc
    grib_fieldset.cc
    grib_filepool.cc
    geo/grib_geography.cc
    grib_handle.cc
    grib_hash_keys.cc
    grib_io.cc
    grib_trie.cc
    grib_trie_with_rank.cc
    grib_itrie.cc
    grib_rules.cc
    grib_keys_iterator.cc
    bufr_keys_iterator.cc
    grib_parse_utils.cc
    grib_query.cc
    grib_scaling.cc
    grib_templates.cc
    grib_dependency.cc
    grib_value.cc
    grib_errors.cc

    grib_arguments.cc

    expression/Expression.cc
    expression/Binop.cc
    expression/LogicalAnd.cc
    expression/LogicalOr.cc
    expression/IsInDict.cc
    expression/True.cc
    expression/StringCompare.cc
    expression/Unop.cc
    expression/Functor.cc
    expression/Accessor.cc
    expression/IsInList.cc
    expression/IsInteger.cc
    expression/Length.cc
    expression/Long.cc
    expression/Double.cc
    expression/String.cc
    expression/SubString.cc

    geo/nearest/Nearest.cc
    geo/nearest/Gen.cc
    geo/nearest/Healpix.cc
    geo/nearest/LambertAzimuthalEqualArea.cc
    geo/nearest/LambertConformal.cc
    geo/nearest/LatlonReduced.cc
    geo/nearest/Mercator.cc
    geo/nearest/PolarStereographic.cc
    geo/nearest/Reduced.cc
    geo/nearest/Regular.cc
    geo/nearest/SpaceView.cc

    geo/iterator/Iterator.cc
    geo/iterator/Gaussian.cc
    geo/iterator/GaussianReduced.cc
    geo/iterator/Gen.cc
    geo/iterator/Healpix.cc
    geo/iterator/LambertAzimuthalEqualArea.cc
    geo/iterator/LambertConformal.cc
    geo/iterator/Latlon.cc
    geo/iterator/LatlonReduced.cc
    geo/iterator/Mercator.cc
    geo/iterator/PolarStereographic.cc
    geo/iterator/Regular.cc
    geo/iterator/SpaceView.cc

    codes_util.cc
    grib_util.cc
    bufr_util.cc
    string_util.cc
    functions.cc
    grib_api_internal.h
    eccodes_prototypes.h
    grib_accessor_factory.cc
    grib_dumper_factory.cc
    grib_iterator_factory.cc
    grib_nearest_factory.cc
    grib_yacc.h
    md5_core.h
    md5_core.cc
    grib_optimize_decimal_factor.cc
    eccodes_windef.h
    # We ship our own generated lex/yacc C files
    grib_yacc.cc  grib_lex.cc )

list( APPEND eccodes_extra_src_files
    grib_bits_fast_big_endian.cc grib_bits_any_endian.cc
    grib_bits_fast_big_endian_vector.cc grib_bits_any_endian_vector.cc
    grib_bits_fast_big_endian_simple.cc grib_bits_any_endian_simple.cc
    grib_bits_fast_big_endian_omp.cc grib_bits_any_endian_omp.cc
    encode_double_array.cc
    grib_bits_ibmpow.cc
    grib_bits_ibmpow_opt.cc )

if(UNIX)
    ecbuild_get_date(eccodes_BUILD_DATE)
endif()
configure_file( grib_api_version.cc.in grib_api_version.cc @ONLY )
configure_file( eccodes_version.h.in eccodes_version.h @ONLY )

# We ship our own generated lex/yacc C files
#ecbuild_generate_yy(YYPREFIX  grib_yy
#                    YACC      griby
#                    LEX       gribl
#                    FLEX_FLAGS " "
#                    LEX_FLAGS  " "
#                    BISON_FLAGS "-y"
#                    YACC_FLAGS  "-y"
#                    DEPENDANT action.cc )

if( HAVE_MEMFS )
    list(APPEND ECCODES_EXTRA_LIBRARIES eccodes_memfs)
endif()

if( eccodes_HAVE_GEOGRAPHY AND eccodes_HAVE_ECKIT_GEO )
    list( APPEND eccodes_src_files
        config/LibEccodes.cc
        config/LibEccodes.h

        geo/BasicAngle.cc
        geo/BasicAngle.h
        geo/EckitMainInit.cc
        geo/EckitMainInit.h
        geo/Geo.cc
        geo/Geo.h
        geo/GeoIterator.cc
        geo/GeoIterator.h
        geo/GribFromSpec.cc
        geo/GribFromSpec.h
        geo/GribToSpec.cc
        geo/GribToSpec.h )
    list( APPEND ECCODES_EXTRA_LIBRARIES eckit_geo )
endif()

ecbuild_add_library( TARGET    eccodes
                     SOURCES   ${CMAKE_CURRENT_BINARY_DIR}/grib_api_version.cc
                              # griby.cc gribl.cc
                              ${eccodes_src_files}
                     #PRIVATE_LIBS      ${ECCODES_EXTRA_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${CMATH_LIBRARIES}
                     PRIVATE_INCLUDES "${AEC_INCLUDE_DIRS}" "${PNG_INCLUDE_DIRS}"
                     PRIVATE_LIBS ${ECCODES_EXTRA_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${AEC_LIBRARIES} ${PNG_LIBRARIES}
                     PUBLIC_LIBS  ${CMATH_LIBRARIES} ${THREADS_LIBRARIES}
                     TEMPLATES ${eccodes_extra_src_files}
                     INSTALL_HEADERS_LIST
                              grib_api.h
                              eccodes.h
                              eccodes_windef.h
                              ${CMAKE_CURRENT_BINARY_DIR}/eccodes_version.h
                              ${PROJECT_BINARY_DIR}/eccodes_config.h
                    PUBLIC_INCLUDES
                              $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/src/eccodes>
                              $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/eccodes>
                              $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/src>
                              $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src> )
