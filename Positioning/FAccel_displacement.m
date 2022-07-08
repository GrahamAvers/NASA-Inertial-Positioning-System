clc; close all; 

FAccel = test;

%dt = DATALOG(:,17);
dt = .05;

V_E = [];V_E(1) = 0;
V_N = [];V_N(1) = 0;
V_U = [];V_U(1) = 0;

P_E = []; P_E(1) = 0;
P_N = []; P_N(1) = 0;
P_U = []; P_U(1) = 0;

for i = 2:length(FAccel)
    V_E(i) = V_E(i-1) + FAccel(i-1,1)*dt;
    V_N(i) = V_N(i-1) + FAccel(i-1,2)*dt;
    V_U(i) = V_U(i-1) + FAccel(i-1,3)*dt;

    P_E(i) = P_E(i-1) + V_E(i-1)*dt;
    P_N(i) = P_N(i-1) + V_N(i-1)*dt;
    P_U(i) = P_U(i-1) + V_U(i-1)*dt; 
end 

plot3(P_E,P_N,P_U)
xlabel("East")
ylabel("North")
zlabel("Up")

