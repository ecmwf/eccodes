!-------------------------
      PROGRAM CONVERT
!
!--------------------------------
!     DESCRIPTION 
!
!     GRIB1 to GRIB2 Conversion
!
!     Author: Jean Nicolau, METEO-FRANCE
!
!--------------------------------
!     DECLARATIONS 
!-------------------------------     
      INTEGER  :: IFILEIN,IFILEOUT,GBID1,GBID2 
      INTEGER  :: IVAL,IC,ICS,IECH,IDAT,IHOUR,INOFIE,IPAR,ITOFFS,ITEMP
      INTEGER  :: NARGS,IRT,ILONG,J,JCOUNT
      INTEGER  :: IPSOPD,IECHLSM,IPTI,ISVOFFS
      INTEGER  :: NI,NJ,INBITS,ISFOFFS,ITOSFS,ISFOSFS,ISVOSFS,IPC,IPN
      INTEGER  :: ILOTR,ITOSP,ITOEF,ITOPD,IPEN
      CHARACTER (LEN=4) :: CPARAM,CECH
      CHARACTER (LEN=20) :: CFILEOUT
      REAL :: RG=9.80665
      REAL(KIND=8) , DIMENSION(:), ALLOCATABLE :: VALUES
      REAL(KIND=8) :: XLAD = -9999.99
      REAL(KIND=8) :: XLOD = -9999.99
      REAL(KIND=8) :: XLAF = -9999.99
      REAL(KIND=8) :: XLOF = -9999.99
      REAL(KIND=8) :: XINCRI = -9999.99
      REAL(KIND=8) :: XINCRJ = -9999.99
      INCLUDE 'grib_api_fortran.h'
!
!***************************************************
!     INITS
!****************************************************
!
      READ(5,*) IPSOPD
      IFILEIN = 15
      IFILEOUT=16
      GBID1 = 0
      ILONG=0
      IC=85
      INOFIE=11
      JCOUNT = 0
      IECHLSM=0
!
!***************************************************
!     OPEN FILES
!****************************************************
!
      IRT =  grib_open_file_ (IFILEIN,"INPUT","r")
      
      IF ( IRT .LT. 0 ) THEN
      WRITE (*,*) "PROBLEME ",IRT," POUR INPUT"
      STOP ' erreur fichier '
      ENDIF
      IVAL=40
      IRT =  grib_open_file_ (IFILEOUT,'OUTPUT',"w")
      JCOUNT = 0

   50 CONTINUE

      JCOUNT = JCOUNT + 1
      WRITE(*,*) "JCOUNT=",JCOUNT
!
!***************************************************
!     READ GRIB1
!****************************************************
!
      IRT=1
      IRT = grib_new_from_file_(IFILEIN,GBID1)
      WRITE (*,*) "code reponse entete grib ",IRT
      IF (IRT.NE. 0 )WRITE (*,*) "code reponse entete grib ",IRT
!
      IF ( GBID1 .LT. 0 ) THEN
        WRITE (*, *) 'total ', JCOUNT ,' ret = ', IRT
        IRT = grib_close_file_(IFILEIN)
        GOTO 60
      ENDIF

!
!     CENTRE
!     ------
!
      IRT = grib_get_int_(GBID1,
     S"identificationOfOriginatingGeneratingSubCentre",ICS)
      IRT = grib_get_int_(GBID1,"indicatorOfParameter",IPAR)
!
!     DATE
!     ----
!
      IF ( IPAR /= 81 ) THEN
        IRT = grib_get_int_(GBID1,"dataDate",IDAT)
        IRT = grib_get_int_(GBID1,"dataTime",IHOUR)
        IRT = grib_get_int_(GBID1,"periodOfTime",IECH)
        IRT = grib_get_int_(GBID1,"periodOfTimeIntervals",IPTI)
	IF ( IPTI .EQ. 0 ) IECHLSM=IECH
      ELSE
        ICS=100
	IECH=IECHLSM
        IPTI=0
      ENDIF       
!
!     PARAMETER
!     ---------
!
      IRT = grib_get_int_(GBID1,"indicatorOfTypeOfLevel",ITOFFS)
      IRT = grib_get_int_(GBID1,"lev",ISVOFFS)
!
!
!     Section 3
!     =========
!
!
      IRT = grib_get_int_(GBID1,"Ni",NI)
      IRT = grib_get_int_(GBID1,"Nj",NJ)
      IRT = grib_get_real8_(GBID1,
     S"latitudeOfFirstGridPointInDegrees",XLAD)
      IRT=grib_get_real8_(GBID1,
     S"longitudeOfFirstGridPointInDegrees",XLOD)
      IRT=grib_get_real8_(GBID1,
     S"latitudeOfLastGridPointInDegrees",XLAF)
      IRT=grib_get_real8_(GBID1,
     S"longitudeOfLastGridPointInDegrees",XLOF)
      IRT=grib_get_real8_(GBID1,"iDirectionIncrementInDegrees",XINCRI)
      IRT=grib_get_real8_(GBID1,"jDirectionIncrementInDegrees",XINCRJ)
!
!     Section 4
!     =========
!

      ILONG = NI*NJ
      IRT=grib_get_int_(GBID1,
     S"numberOfBitsContainingEachPackedValue",INBITS)

      IF(.NOT.ALLOCATED(VALUES)) ALLOCATE(VALUES(ILONG))
 
      IRT=grib_get_real8_array_(GBID1,"values",VALUES,ILONG)

!
!************************************************************
!
!    WRITE GRIB2
!    --------------
!
!************************************************************
!
      IRT= grib_new_from_samples_(GBID2,"GRIB2")
      ISFOFFS=-9999
      IRT=grib_set_int_(GBID2,"editionNumber",2)
      IRT=grib_set_int_(GBID2,"dataRepresentationTemplateNumber",IVAL)
      IRT = grib_set_int_(GBID2,
     S"identificationOfOriginatingGeneratingCentre",IC)
      IRT = grib_set_int_(GBID2,
     S"identificationOfOriginatingGeneratingSubCentre",ICS)
      IRT=grib_set_int_(GBID2,"productionStatusOfProcessedData",IPSOPD)
      IRT = grib_set_int_(GBID2,"dataDate",IDAT)
      IRT = grib_set_int_(GBID2,"dataTime",IHOUR)
      IRT=grib_set_int_(GBID2,"shapeOfTheEarth",6)
      IRT=grib_set_int_(GBID2,"dataRepresentationType",0)
      IRT=grib_set_int_(GBID2,"numberOfPointsAlongAParallel",NI)
      IRT=grib_set_int_(GBID2,"fieldWidth",NI)
      IRT=grib_set_int_(GBID2,"numberOfPointsAlongAMeridian",NJ)
      IRT=grib_set_int_(GBID2,"fieldHeight",NJ)
      IRT=grib_set_real8_(GBID2,
     S"latitudeOfFirstGridPointInDegrees",XLAD)
      IF(XLOD.LT.0.0) XLOD= 360.00 + XLOD
      IRT=grib_set_real8_(GBID2,
     S"longitudeOfFirstGridPointInDegrees",XLOD)
      IRT=grib_set_real8_(GBID2,
     S"latitudeOfLastGridPointInDegrees",XLAF)
      IF(XLOF.LT.0.0) XLOF= 360.00 + XLOF
      IRT=grib_set_real8_(GBID2,
     S"longitudeOfLastGridPointInDegrees",XLOF)
      IRT=grib_set_real8_(GBID2,"iDirectionIncrementInDegrees",XINCRI)
      IRT=grib_set_real8_(GBID2,"jDirectionIncrementInDegrees",XINCRJ)
!
!
!     Section 4
!     =========
!

      ITEMP=1
      ITOSFS=255
      ISFOSFS=255
      ISVOSFS=255
      SELECT CASE (IPAR)
        CASE (33)
          IPC=2
          IPN=2
          SELECT CASE (ITOFFS)
            CASE(105)
              CPARAM="10u"
              ITOFFS=103
              ISFOFFS=0
            CASE (100)
              ISFOFFS=0
              ISVOFFS=ISVOFFS*100
              CPARAM="u"
            CASE (117)
              CPARAM="u"
              ITOFFS=109
              ISFOFFS=6
              ISVOFFS=ISVOFFS/1000
          END SELECT
        CASE (34)
          IPC=2
          IPN=3
          SELECT CASE (ITOFFS)
            CASE(105)
              CPARAM="10v"
              ITOFFS=103
              ISFOFFS=0
            CASE (100)
              ISFOFFS=0
              ISVOFFS=ISVOFFS*100
              CPARAM="v"
            CASE (117)
              CPARAM="v"
              ITOFFS=109
              ISFOFFS=6
              ISVOFFS=ISVOFFS/1000
          END SELECT
      CASE (11)
          IPC=0
          IPN=0
        SELECT CASE (ITOFFS)
          CASE (111)
            CPARAM="st"
            IPN=2
            ITOFFS=106
            ISVOFFS=0
            ITOSFS=106
            ISFOFFS=0
            ISFOSFS=1
            ISVOSFS=2
            IRT=grib_set_int_(GBID2,"discipline",2)
          CASE(105)
            CPARAM="2t"
            IPN=0
            ITOFFS=103
            ISFOFFS=0
            ISVOFFS=2
            IRT=grib_set_int_(GBID2,"binaryScaleFactor",-4)
            IRT=grib_set_int_(GBID2,"decimalScaleFactor",0)
          CASE      (100)
            IPN=0
            ISFOFFS=0
            ISVOFFS=ISVOFFS*100
            CPARAM="t"
          CASE      (1)
            CPARAM="skt"
            ITOSFS=255
            IPN=17
        END      SELECT
      CASE      (114)
        CPARAM="ttr"
        ITOFFS=8
        IPC=5
        IPN=5
      CASE      (8)
        CPARAM="orog"
        IPC=3
        IPN=5
      CASE      (17)
        CPARAM="2d"
        IPC=0
        IPN=6
        ITOFFS=103
        ISFOFFS=0
        ISVOFFS=2
        IRT=grib_set_int_(GBID2,"binaryScaleFactor",-9)
        IRT=grib_set_int_(GBID2,"decimalScaleFactor",-2)
      CASE      (51)
        CPARAM="q"
        IPC=1
        IPN=0
        ISFOFFS=0
        ISVOFFS=ISVOFFS*100
        IRT=grib_set_int_(GBID2,"binaryScaleFactor",-22)
        IRT=grib_set_int_(GBID2,"decimalScaleFactor",-2)
      CASE      (6)
        CPARAM="gh"
        VALUES=VALUES/RG
        ISFOFFS=0
        ISVOFFS=ISVOFFS*100
        IPC=3
        IPN=5
      CASE      (13)
        CPARAM="pt"
          IPC=0
          IPN=2
          ITOFFS=109
          ISFOFFS=6
          ISVOFFS=ISVOFFS/1000
      CASE      (2)
        CPARAM="msl"
        IPC=3
        IPN=0
        ITOFFS=101
      CASE      (71)
        CPARAM="tcc"
        IPC=6
        IPN=1
        ITOSFS=8
        IRT=grib_set_int_(GBID2,"binaryScaleFactor",-7)
        IRT=grib_set_int_(GBID2,"decimalScaleFactor",-2)
      CASE      (65)
        CPARAM="sd"
	ITOSFS=255
        IPC=1
        IPN=60
      CASE      (61)
        CPARAM="tp"
        ITOSFS=255
        IPC=1
        IPN=52
      CASE      (99)
        CPARAM="sf"
        ITOSFS=255
        IPC=1
        IPN=53
      CASE      (1)
        CPARAM="sp"
	ITOSFS=255
        IPC=3
        IPN=0
      CASE      (160)
        CPARAM="cape"
        ITOSFS=8
        IPC=7
        IPN=6
      CASE      (122)
        CPARAM="sshf"
        ITOSFS=255
        IPC=0
        IPN=11
      CASE      (121)
        CPARAM="slhf"
        ITOSFS=255
        IPC=0
        IPN=10
      CASE      (112)
        CPARAM="str"
        ITOSFS=255
        IPC=5
        IPN=5
      CASE      (111)
        CPARAM="ssr"
        ITOSFS=255
        IPC=4
        IPN=9
      CASE      (167)
        CPARAM="tcw"
        ITOSFS=8
        IPC=1
        IPN=51
        ITOSFS=8
      CASE      (81)
        CPARAM="lsm"
        IRT=grib_set_int_(GBID2,"discipline",2)
        IPC=0
        IPN=0
      END SELECT
      ILOTR=0
      ITOSP=0
      IF ( IPTI .GT. 0 ) THEN
        ITOSP=1
        ITEMP=11
        ITOSP=1
        ILOTR=IPTI
	IRT = grib_set_int_(GBID2,"productDefinitionTemplateNumber",ITEMP)
	IRT = grib_set_int_(GBID2,"marsStartStep",0)
	IRT = grib_set_int_(GBID2,"marsEndStep",IPTI)
      ELSE
	ITEMP=1
	IRT = grib_set_int_(GBID2,"productDefinitionTemplateNumber",ITEMP)
	IRT = grib_set_int_(GBID2,"forecastTime",IECH)
        IRT = grib_set_int_(GBID2,"marstStep",IECH)
      ENDIF
      IRT=grib_set_int_(GBID2,"typeOfStatisticalProcessing",ITOSP)
      IRT = grib_set_int_(GBID2,"typeOfGeneratingProcess",4)
      IRT=grib_set_int_(GBID2,"generatingProcessIdentifier",211)
      IF ( ICS .EQ. 100 .OR. ICS .EQ. 0 ) THEN
        ITOEF=1
        ITOPD=3
        IPEN=0
      ELSE
        ITOEF=3
        ITOPD=4
        IPEN=ICS-100
      ENDIF
      IRT=grib_set_int_(GBID2,"typeOfEnsembleForecast",ITOEF)
      IRT=grib_set_int_(GBID2,"typeOfProcessedData",ITOPD)
      IRT=grib_set_int_(GBID2,"perturbationNumber",IPEN)
      IRT=grib_set_int_(GBID2,"numberOfForecastsInEnsemble",INOFIE)
      IRT=grib_set_int_(GBID2,"parameterCategory",IPC)
      IRT=grib_set_int_(GBID2,"parameterNumber",IPN)
      IRT=grib_set_int_(GBID2,"typeOfFirstFixedSurface",ITOFFS)
      IF ( ISFOFFS /= -9999 ) THEN
      IRT=grib_set_int_(GBID2,"scaleFactorOfFirstFixedSurface",ISFOFFS)
      IRT=grib_set_int_(GBID2,"scaledValueOfFirstFixedSurface",ISVOFFS)
      ENDIF
      IRT=grib_set_int_(GBID2,"typeOfSecondFixedSurface",ITOSFS)
      IF ( ISFOSFS .NE. 255 )  IRT=grib_set_int_(GBID2,
     S      "scaleFactorOfSecondFixedSurface",ISFOSFS)
      IF ( ISVOSFS .NE. 255 ) IRT=grib_set_int_(GBID2,
     S      "scaledValueOfSecondFixedSurface",ISVOSFS)
      IRT=grib_set_int_(GBID2,
     S"numberOfBitsContainingEachPackedValue",INBITS)
      IRT=grib_set_real8_array_(GBID2,"values",VALUES,ILONG)
      IRT = grib_write_(GBID2,IFILEOUT)
      IRT = grib_release_(GBID1)
!
!
      DEALLOCATE(VALUES)
!
      GO TO 50
   60 CONTINUE  
!

      IRT = grib_close_file_(IFILEOUT)
!
      END
