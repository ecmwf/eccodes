# Copyright 2005-2015 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#
# Python implementation:  bufr_read_tropical_cyclone
#
# Description: how to read data of the ECMWF EPS tropical cyclone tracks encoded in BUFR format.
#
# Please note that scatterometer data can be encoded in various ways in BUFR. Therefore the code
# below might not work directly for other types of messages than the one used in the
# example. It is advised to use bufr_dump first to understand the structure of these messages.
#


import traceback
import sys
import collections
#import numpy

from eccodes import *

INPUT = '../../data/bufr/tropical_cyclone.bufr'
VERBOSE = 1  # verbose error reporting


data=collections.defaultdict(dict)
def example():

    # open bufr file
    f = open(INPUT)

    cnt = 1

    # loop for the messages in the file
    while 1:
        # get handle for message
        gid = codes_bufr_new_from_file(f)
        if gid is None:
            break

        print '**************** MESSAGE: ',cnt,'  *****************'

        # we need to instruct ecCodes to expand all the descriptors
        # i.e. unpack the data values
        codes_set(gid, 'unpack', 1)

# The BUFR file contains a single message with number of subsets in a compressed form.
# It means each subset has exactly the same structure.
# One subset contains values of latitude, longitude, pressure, wind at 10m
# for one particular ensemble member over forecast period from 0h to 240h by 6h step.
# To print values of latitude, longitude, pressure, wind at 10m for particular ensemble member from all the subsets
# we will simply access the key by condition (see below)
#
# The key latitude will give back the array of all the values corresponding
# to all the instances of the key in the tree.
# The key #2#latitude will return only the values of the second instance in the tree.


        numObs=codes_get(gid,"numberOfSubsets")
        year  = codes_get(gid, "year")
        month = codes_get(gid, "month")
        day   = codes_get(gid, "day")
        hour  = codes_get(gid, "hour")
        minute= codes_get(gid, "minute")
      
        #print 'Date and time: %d%s %d%s %d%s %d%s %d' % (day,'.',month,'.',year,'  ',hour,':',minute)  
        print 'Date and time: ',  day,'.',month,'.',year,'  ',hour,':',minute  


        stormIdentifier =  codes_get(gid,"stormIdentifier")
        print  'Storm identifier: ', stormIdentifier
   

        #How many different timePeriod in the data structure?
        rankPeriod=0
        while True:
            rankPeriod=rankPeriod+1
            try:
                codes_get_array(gid,"#%d#timePeriod" %rankPeriod)
                #period=next(timeStep[i] for i in range (len(timeStep))if timeStep[i] != CODES_MISSING_DOUBLE )
            except CodesInternalError, err:
                #print err.msg
                #del(period) 
                break
            #the numberOfPeriods includes the analysis (period=0)

        # Get ensembleMemberNumber
        memberNumber = codes_get_array(gid, "ensembleMemberNumber")
        memberNumberLen=len(memberNumber)
   
 
        # Observed Storm Centre
        significance    = codes_get(gid,'#1#meteorologicalAttributeSignificance')
        latitudeCentre  = codes_get(gid,'#1#latitude')
        longitudeCentre = codes_get(gid,'#1#longitude')

        if (significance!=1):
            print 'ERROR: unexpected #1#meteorologicalAttributeSignificance'
            return 1

        if (latitudeCentre==CODES_MISSING_DOUBLE) and (longitudeCentre==CODES_MISSING_DOUBLE):
           print 'Observed storm centre position missing'
        else:
           print 'Observed storm centre: latitude=',latitudeCentre,' longitude=',longitudeCentre

        # Location of storm in perturbed analysis
        significance = codes_get(gid,'#2#meteorologicalAttributeSignificance')
        latitudeAnalysis = codes_get_array(gid,'#2#latitude')
        longitudeAnalysis = codes_get_array(gid,'#2#longitude')
        pressureAnalysis = codes_get_array(gid,'#1#pressureReducedToMeanSeaLevel')

        if (significance!=4):
            print 'ERROR: unexpected #2#meteorologicalAttributeSignificance'
            return 1

        latitude=[[0 for x in range(rankPeriod)] for x in range(len(memberNumber))] 
        longitude=[[0 for x in range(rankPeriod)] for x in range(len(memberNumber))] 
        pressure=[[0 for x in range(rankPeriod)] for x in range(len(memberNumber))] 


        if (len(latitudeAnalysis)==len(memberNumber)):
            for i in range(len(memberNumber)):
                latitude[i][0]=latitudeAnalysis[i]
                longitude[i][0]=longitudeAnalysis[i]
                pressure[i][0]=pressureAnalysis[i]
                
        else:
            for i in range(len(memberNumber)):
                latitude[i][0]=latitudeAnalysis[0]
                longitude[i][0]=longitudeAnalysis[0]
                pressure[i][0]=pressureAnalysis[i]

        # Location of Maximum Wind
        significance=codes_get(gid,'#3#meteorologicalAttributeSignificance')
        latitudeMaxWind0=codes_get_array(gid,'#3#latitude')
        longitudeMaxWind0= codes_get_array(gid,'#3#longitude')

        if (significance!=3):
            print 'ERROR: unexpected #3#meteorologicalAttributeSignificance=', significance
            return 1

        latitudeWind=[[0 for x in range(rankPeriod)] for x in range(len(memberNumber))] 
        longitudeWind=[[0 for x in range(rankPeriod)] for x in range(len(memberNumber))] 
        wind=[[0 for x in range(rankPeriod)] for x in range(len(memberNumber))] 

        windMaxWind0= codes_get_array(gid,'#1#windSpeedAt10M')

        if (len(latitudeMaxWind0)==len(memberNumber)):
            for i in range(len(memberNumber)):
                latitudeWind[i][0]=latitudeMaxWind0[i]
                longitudeWind[i][0]=longitudeMaxWind0[i]
                wind[i][0]=windMaxWind0[i]

        else:
            for i in range(len(memberNumber)):
                latitudeWind[i][0]=latitudeMaxWind0[0]
                longitudeWind[i][0]=longitudeMaxWind0[0]
                wind[i][0]=windMaxWind0[i]

        rankSignificance=3
        rankPosition=3
        rankPressure=1
        rankWind=1
 
        timePeriod=[0 for x in range(rankPeriod)]
        for i in range(1,rankPeriod):
        #for i in range(rankPeriod):
            rank1 = i * 2 + 2
            rank3 = i * 2 + 3

            #rank1 = (i+1) * 2 + 2
            #rank3 = (i+1)* 2 + 3

            ivalues= codes_get_array(gid,"#%d#timePeriod" %(i))

            if (len(ivalues)==1):
                timePeriod[i]=ivalues[0] 
            else:
               for j in range(len (ivalues)): 
                   if (ivalues[j]!=CODES_MISSING_LONG):
                       timePeriod[i]=ivalues[j]
                       break 

            values = codes_get_array(gid, "#%d#meteorologicalAttributeSignificance" % rank1)
            if (len(values)==1):
                significance=values[0] 
            else:
               for j in range(len (values)): 
                   if (values[j]!=CODES_MISSING_LONG):
                       significance=values[j]
                       break 

            if(significance==1):
                lat = codes_get_array(gid, "#%d#latitude" % rank1)
                lon = codes_get_array(gid, "#%d#longitude" % rank1)
                press = codes_get_array(gid, "#%d#pressureReducedToMeanSeaLevel" % (i + 1))
            else:
                print 'ERROR: unexpected meteorologicalAttributeSignificance=',significance

            
            values = codes_get_array(gid, "#%d#meteorologicalAttributeSignificance" % rank3)
            if (len(values)==1):
                significanceWind=values[0] 
            else:
               for j in range(len (values)): 
                   if (values[j]!=CODES_MISSING_LONG):
                       significanceWind=values[j]
                       break 

            if(significanceWind==3):
                lat3 = codes_get_array(gid, "#%d#latitude" % rank3)
                lon3 = codes_get_array(gid, "#%d#longitude" % rank3)
                wind3 = codes_get_array(gid, "#%d#windSpeedAt10M" % (i + 1))
            else:
                print 'ERROR: unexpected meteorologicalAttributeSignificance=',significanceWind
     
            #for k in range(1,len(memberNumber)):
            for k in range(len(memberNumber)):
                data[k][i]=[lat[k],lon[k],press[k],lat3[k],lon3[k],wind3[k]]
            
            if(cnt==2):
                print 'majaaa', i,timePeriod[s] 

 # ---- Print the values --------------------------------

        for m in sorted( data.keys()):
            print "== Member  %d"%(m+1)
            print "step  latitude  longitude   pressure  latitude   longitude    wind"
            #print " {:<3d}{:2.2f}{:2.2f}{}{} {} ".format(timePeriod[0],latitude[m][0],longitude[m][0],pressure[m][0],latitudeWind[m][0],longitudeWind[i][0],wind[i][0])
            if latitude[m][0]!=CODES_MISSING_DOUBLE:
                print timePeriod[0],latitude[m][0],longitude[m][0],pressure[m][0], 
                #print " {:>3d}{}{:>6.1f}{}{:>6.1f}{}{:>8.1f}{}{:>6.1f}{}{:>6.1f}{}{:>6.1f}".format(\
                #      timePeriod[0],'  ',latitude[m][0],'     ',longitude[m][0],'     ',pressure[m][0],'  ',latitudeWind[m][0],'     ',longitudeWind[m][0],'     ',wind[m][0])

            for s in sorted( data[m].keys()):
                if data[m][s][0]!=CODES_MISSING_DOUBLE:
                    print 'maja',s
                    #print " {:>3d}{}{:>6.1f}{}{:>6.1f}{}{:>8.1f}{}{:>6.1f}{}{:>6.1f}{}{:>6.1f}".format(\
                    #      timePeriod[s],'  ',data[m][s][0],'     ',data[m][s][1],'     ',data[m][s][2],'  ',data[m][s][3],'     ',data[m][s][4],'     ',data[m][s][5])
                    #print "%d  %.6g %.6g"%(timePeriod[s],data[m][s][0],data[m][s][1])



        codes_release(gid)

    cnt += 1
    print 'majaaaaaaaaaaa',cnt

    #del(timePeriod)
    #del(latitude)
    #del(longitude)
    #del(pressure)
    #del(latitudeWind)
    #del(longitudeWind)
    #del(wind)
    #del(data)

        # close the file
    f.close()


def main():
    try:
        example()
    except CodesInternalError, err:
        if VERBOSE:
            traceback.print_exc(file=sys.stderr)
        else:
            print >>sys.stderr, err.msg

        return 1

if __name__ == "__main__":
    sys.exit(main())
