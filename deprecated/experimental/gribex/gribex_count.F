      PROGRAM GRDEMO
C
      IMPLICIT NONE
      INTEGER JPACK, JPBYTE
C
      PARAMETER (JPACK = 52000000)
      PARAMETER (JPBYTE = 4)
      INTEGER ISEC0 (2)
      INTEGER ISEC1 (1024)
      INTEGER ISEC2 (1024)
      INTEGER ISEC3 (2)
      INTEGER ISEC4 (512)
      REAL ZSEC2 (512)
      REAL ZSEC3 (2)
      REAL ZSEC4 (JPACK * 4)
      INTEGER INBUFF (JPACK)
      CHARACTER*256 YDATAFILE
      CHARACTER*2 INPUT
      CHARACTER*1 YOPER
      INTEGER IFILE, NUMERR, IPBLEN, IWORD, IFILEN, IRET, IARGC, NARGS
      INTEGER JCOUNT, LENOUT
      INTEGER JLAT, JLONG, JSTART, JEND
      REAL ZFIRST, ZLAST, ZSTEP, ZLAT
      REAL ZMAX,ZMIN

      NARGS = IARGC()
      IF( NARGS.LT.2 ) THEN
         print*,'Usage: gribex_check -i inputfile'
         STOP
      END IF

      CALL GETARG(1,INPUT)

      IF(INPUT.EQ.'-i') THEN
         CALL GETARG(2,YDATAFILE)
      ELSE
         print*,'Usage: gribex_check -i inputfile' 
         STOP
      END IF

      IPBLEN = JPACK * JPBYTE
      IFILEN = INDEX(YDATAFILE,' ') - 1
      CALL PBOPEN (IFILE, YDATAFILE (1: IFILEN), 'R', IRET)
      IF ( IRET .NE. 0 ) THEN
        WRITE (*, *) ' Return code from PBOPEN = ', IRET
        CALL PBCLOSE(IFILE, IRET)
        STOP 'Fault in PBOPEN'
      ENDIF
C
C     Loop through GRIB products in file.
      JCOUNT = 0
C
   50 CONTINUE
      JCOUNT = JCOUNT + 1
C
C     Read packed field into INBUFF.
      CALL PBGRIB (IFILE, INBUFF, IPBLEN, LENOUT, IRET )
C
      IF ( IRET .LT. 0 ) THEN
c       WRITE (*, *) ' Return code from pbgrib = ', IRET
        IF ( IRET .EQ. -1) THEN
          WRITE(*,'(I6)') (JCOUNT-1)
          CALL PBCLOSE (IFILE, IRET)
          GO TO 100
        ELSE
          WRITE (*, *) ' kret = ',IRET,' after ', JCOUNT,' products.'
          CALL PBCLOSE (IFILE, IRET)
          STOP 'Fault in PBGRIB'
        ENDIF
      ENDIF
C
C     'D' function to unpack entire GRIB message.
C
      YOPER = 'D'
C     WRITE (*,*) ' GRDEMO : Function code = ', YOPER
C
      IRET = 1
      LENOUT = LENOUT / JPBYTE
C
C     Set missing data values
C
      ISEC3 (2) = -99999
      ZSEC3 (2) = -99999.0
C
      CALL GRIBEX (ISEC0, ISEC1, ISEC2, ZSEC2, ISEC3, ZSEC3, ISEC4,
     X             ZSEC4, IPBLEN, INBUFF, LENOUT, IWORD, YOPER, IRET)
C
C     Check return code.
C
C     WRITE (*,*) ' GRDEMO : GRIBEX return code = ', IRET
      IF (IRET .EQ. - 6) WRITE (*,*) ' GRDEMO : Pseudo-grib data found.'
      IF (IRET .GT. 0) THEN
        IF ( IRET .EQ. 413 ) THEN
          JCOUNT=JCOUNT-1
        ELSE
          WRITE (*, *) ' kret = ',IRET,' after ', JCOUNT,' products.'
          STOP 'Fault in GRIBEX'
        ENDIF
      ENDIF
      GO TO 50
C
100   END
