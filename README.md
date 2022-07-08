# Robotrigo
## How to test
1. Danger: High voltage on servo controller, power supply and motor!!!! Red switched on rail kills high voltage.
1. Make sure motor is clampt 
1. Turn little toggle switch on. If andy problem accures - turn it off. 
1. Put target metal agains limit senstors
1. Send Pump on - Red led comes on
1. Send Power on - Green led on and system comes up
1. All variables are initialized to 0, and hold last sucssesful measurment value
1. Pressure sensore is alwayes on, and should give: 270~280.
1. Send servo on 
1. Send forwad, reverse and stop commands. observe variables
1. Remove target from limit sensors
1. Error comes on the black controller and motor stops
1. Return target to limit switch
1. send pclear
1. motor resumes
1. send servo off. motor stops
1. pump off, power off
1. reset test - t.b.d
  