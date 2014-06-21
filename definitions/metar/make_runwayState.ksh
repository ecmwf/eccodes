p4 edit runwayState2.def
cat runwayState1.def | sed 's/State1/State2/g' > runwayState2.def
p4 edit runwayState3.def
cat runwayState1.def | sed 's/State1/State3/g' > runwayState3.def
p4 edit runwayState4.def
cat runwayState1.def | sed 's/State1/State4/g' > runwayState4.def
