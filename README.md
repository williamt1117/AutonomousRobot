# AutonomousRobot
Code written for RobotC used to operate an autonomous robot that represents a dry-lab test of underwater ocean-sensor cleaning.
<img width="354" alt="image" src="https://user-images.githubusercontent.com/92940760/196819655-0ab79fab-1219-437d-b7a5-216bdf8a8f68.png">
[Figure 1] Isometric View of Robot

Code functions off of phases all represented in Figure 2 below.
<img width="826" alt="image" src="https://user-images.githubusercontent.com/92940760/196819549-382ea58c-55ee-43e8-85eb-c813cb3c4307.png">
[Figure 2] FSM Diagram of robot

## Approaching Phase
The approaching phase consists of a loop between approaching the target and facing the target. The angle adjustment can be broken down into 4 pieces as shown in figure 5. The first phase the robot rotates counter clockwise until infrared is no longer detected. Due to the IR light on the target flashing at a rate of 10Hz for IR to be effectively “no longer detected” the robot must wait till IR has not been detected for a span of 0.2 seconds. The first phase will not run if IR is not visible when this phase is called. The second phase will rotate clockwise and note the angle in which IR light is first detected. The robot continues to rotate without pausing through to the third phase where it rotates clockwise until IR is no longer detected in a similar fashion to the first phase. When the third phase completes it notes the angle again. The fourth and final phase the robot takes the difference of the 2 angles and rotates counter clockwise by half that amount effectively facing the target represented by the gray line in figure 3. After phase 4 the robot moves to the approaching the target function. Firstly, the robot simply drives forwards for 1 second and if the ultrasonic sensor detects the target within a range of 16 centimetres, then the robot stops driving and moves on to the proximity adjusting phase. If the target is not within range at the end of the approaching target phase, then the robot returns to the angle adjustment phase.

<img width="701" alt="image" src="https://user-images.githubusercontent.com/92940760/196819490-1fd171e0-5405-4e97-8f64-52610d0f111a.png">
[Figure 3] Rotate to target function implementation

##Proximity Adjustment Phase
The proximity adjustment phase is used to account for the rolling of the robot due to inertia after the approaching phase ends using exclusively the ultrasonic range finder and drive wheels. This phase will immediately exit if the distance recorded by the ultrasonic sensor is exactly equal to the ideal target distance, 17cm. If the robot is further away than 17cm then it will drive forward slowly until the distance is equal to 17cm and vice versa for closer than 17cm. After this, the robot moves to the dropping phase. 

##Dropping Phase
The dropping phase is a linear system of commands and delays. When this completes the robot transfers to the retrieving phase. It follows the following procedure: Firstly, main arm dropped to be flush with the target / until limit switch is hit. Secondly, econdary motor opens the cage to allow the ping pong ball to sit on the target. Thirdly, main arm is lifted to avoid bumping the ping pong ball. Lastly, secondary motor closes cage.

##Retrieval Phase
The retrieval phase consists of another linear system of commands which takes on the following procedure: Firstly, drives backwards for 2 seconds to exit “danger area” of bumping target. Secondly, rotates 180 degrees. Thirdly, drives forward until sonar detects wall or front bumper switches are pressed. Lastly, signals completion via an LED
