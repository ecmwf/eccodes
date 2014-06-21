p4 edit rvr2.def
cat rvr1.def | sed 's/RVR1/RVR2/g' > rvr2.def
p4 edit rvr3.def
cat rvr1.def | sed 's/RVR1/RVR3/g' > rvr3.def
p4 edit rvr4.def
cat rvr1.def | sed 's/RVR1/RVR4/g' > rvr4.def
