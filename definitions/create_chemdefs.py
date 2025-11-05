#!/usr/bin/env python3

import pandas as pd
import os
import sys
import subprocess
import mysql.connector
## Create a connection object
## IP address of the MySQL database server
Host = os.environ['PARAM_DB_HOST']
## User name of the database server
User = os.environ['PARAM_DB_USER']
## Password for the database user
Password = os.environ['PARAM_DB_PASS']
## opens directly the database param
database = "param"

conn  = mysql.connector.connect(host=Host, user=User, password=Password, database=database)

# Create a cursor object
cur  = conn.cursor()

# WMO definitions
#os.system('mkdir -p grib2')
centre = 0
query = f"select chem.id as chemId,shortName as chemShortName,chem.name as chemName,chem.formula as chemFormula,group_concat(concat(attribute.name,'=',coalesce(chem_attributes.attribute_value,'missing()')) order by attribute.o) as attributes from chem join chem_attributes on chem_attributes.chem_id=chem.id join attribute on attribute.id=chem_attributes.attribute_id where chem.centre_id={centre} group by chem.id;"
#cur.execute(query)
#df = cur.fetchall()
df = pd.read_sql(query, con=conn)
f1 = open("grib2/chemId.def", "w")
f2 = open("grib2/chemName.def", "w")
f3 = open("grib2/chemShortName.def", "w")
f4 = open("grib2/chemFormula.def", "w")
for i in range(len(df)):
    attr_i = df.iloc[i]['attributes'].split(',')
    chemName = df.iloc[i]['chemName']
    chemId = df.iloc[i]['chemId']
    chemShortName = df.iloc[i]['chemShortName']
    chemFormula = df.iloc[i]['chemFormula']
    print(chemId,chemName)
#    os.system('echo "#' + str(chemName) + '"')
    f1.write(f"#{chemName}\n")
    f2.write(f"#{chemName}\n")
    f3.write(f"#{chemName}\n")
    f4.write(f"#{chemName}\n")
#    os.system("echo \"" + "'" + str(chemId) + "'\" = {")
    f1.write("'" + f"{chemId}" + "' " +  "=" + " {" + "\n")
    f2.write("'" + f"{chemName}" + "' " +  "=" + " {" + "\n")
    f3.write("'" + f"{chemShortName}" + "' " +  "=" + " {" + "\n")
    f4.write("'" + f"{chemFormula}" + "' " +  "=" + " {" + "\n")
#    os.system(f"echo \#{chemName}")
    # we loop over the attributes
    for j in range(len(attr_i)):
        aid = attr_i[j].split('=')[0]
        ava = attr_i[j].split('=')[1]
#        os.system('echo "\t' + str(aid) + ' = ' + str(ava) + ' ;"')
        f1.write(f"\t{aid} = {ava} ;\n")
        f2.write(f"\t{aid} = {ava} ;\n")
        f3.write(f"\t{aid} = {ava} ;\n")
        f4.write(f"\t{aid} = {ava} ;\n")
#    os.system('echo "\t"' + "}")
    f1.write(f"\t" + '}\n')
    f2.write(f"\t" + '}\n')
    f3.write(f"\t" + '}\n')
    f4.write(f"\t" + '}\n')
f1.close()
f2.close()
f3.close()
f4.close()

# ECMWF definitions
#os.system('mkdir -p grib2/localConcepts/')
#os.system('mkdir -p grib2/localConcepts/ecmf/')
centre = 98
query = f"select chem.id as chemId,shortName as chemShortName,chem.name as chemName,chem.formula as chemFormula,group_concat(concat(attribute.name,'=',coalesce(chem_attributes.attribute_value,'missing()')) order by attribute.o) as attributes from chem join chem_attributes on chem_attributes.chem_id=chem.id join attribute on attribute.id=chem_attributes.attribute_id where chem.centre_id={centre} group by chem.id;"
#cur.execute(query)
#df = cur.fetchall()
df = pd.read_sql(query, con=conn)
f1 = open("grib2/localConcepts/ecmf/chemId.def", "w")
f2 = open("grib2/localConcepts/ecmf/chemName.def", "w")
f3 = open("grib2/localConcepts/ecmf/chemShortName.def", "w")
f4 = open("grib2/localConcepts/ecmf/chemFormula.def", "w")
for i in range(len(df)):
    attr_i = df.iloc[i]['attributes'].split(',')
    chemName = df.iloc[i]['chemName']
    chemId = df.iloc[i]['chemId']
    chemShortName = df.iloc[i]['chemShortName']
    chemFormula = df.iloc[i]['chemFormula']
    print(chemId,chemName)
#    os.system('echo "#' + str(chemName) + '"')
    f1.write(f"#{chemName}\n")
    f2.write(f"#{chemName}\n")
    f3.write(f"#{chemName}\n")
    f4.write(f"#{chemName}\n")
#    os.system("echo \"" + "'" + str(chemId) + "'\" = {")
    f1.write("'" + f"{chemId}" + "' " +  "=" + " {" + "\n")
    f2.write("'" + f"{chemName}" + "' " +  "=" + " {" + "\n")
    f3.write("'" + f"{chemShortName}" + "' " +  "=" + " {" + "\n")
    f4.write("'" + f"{chemFormula}" + "' " +  "=" + " {" + "\n")
#    os.system(f"echo \#{chemName}")
    # we loop over the attributes
    for j in range(len(attr_i)):
        aid = attr_i[j].split('=')[0]
        ava = attr_i[j].split('=')[1]
#        os.system('echo "\t' + str(aid) + ' = ' + str(ava) + ' ;"')
        f1.write(f"\t{aid} = {ava} ;\n")
        f2.write(f"\t{aid} = {ava} ;\n")
        f3.write(f"\t{aid} = {ava} ;\n")
        f4.write(f"\t{aid} = {ava} ;\n")
#    os.system('echo "\t"' + "}")
    f1.write(f"\t" + '}\n')
    f2.write(f"\t" + '}\n')
    f3.write(f"\t" + '}\n')
    f4.write(f"\t" + '}\n')
f1.close()
f2.close()
f3.close()
f4.close()

conn.close()
