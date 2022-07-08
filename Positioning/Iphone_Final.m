% Test Using Iphone Recorded Data %
% Linear Acceleration Values %
% units are in SI %
% Angular Velocities in Rad/s %

[numRows,numCols] = size(Accelerations);
%AngularVelocitee = timetable2table(AngularVelocity,'ConvertRowTimes',false);
%AngularVelocitee = AngularVelocitee{:,:};
%Accelerations = timetable2table(Acceleration,'ConvertRowTimes',false);
%Accelerations = Accelerations{:,:};
%Orientations = timetable2table(Orientation,'ConvertRowTimes',false);
%Orientations = Orientations{:,:};

v=1;
XSpeed = 0;
YSpeed = 0;
XPos = 0;
YPos = 0;
XSpin = 0; %in radians
YSpin = 0; %in radians
ZSpin = 0; %in radians
a = 0;

% Column 1 = X, Column 2 = Y, Column 3 = Z
FinalAccelerations = zeros(numRows,3);


while numRows > 1
    a = a+1;
   Orientations(a,1) = Orientations(a,1)+90;
% degree off zero calculation (unused with Iphone as it calculates itself)
    %ZSpin = ZSpin + AngularVelocitee(a,3)*.01*180/pi; % in radians, 
   % YSpin = YSpin + AngularVelocitee(a,2)*.01*180/pi; % in radians (100hz 1second/100hz = .01 s)
  %  XSpin = XSpin + AngularVelocitee(a,1)*.01*180/pi; % in radians

% calculating correct acceleration  YET TO BE CALCULATED:: centripitel Acceleration,
    
        % adding .12 as calibration factor for iphone x acceleromter
        % Orientations(a,2) is pitch degree's


%------------------------------------------------------------------------%


                        %% X ACCELERATION CALCULATIONS  
    %.138 is X acceleration calibration of iphone

    if Orientations(a,3) > 0   % if roll > 0
        % if theta > 0 add 9.81sind(theta) to remove gravity acceleration component
        XMeasure = Accelerations(a,1)+.138+9.81*sind(Orientations(a,3));
         
    else   
        % if theta < 0 subtract 9.81sind(theta) to remove gravity acceleration component
        XMeasure = Accelerations(a,1)+.138-9.81*sind(Orientations(a,3));
        
    end

        XPitch = (XMeasure/cosd(Orientations(a,2)))^2;
        XAzimuth = (XMeasure/cosd(Orientations(a,1)))^2;
        FinalAccelerations(a,1) = sqrt(XPitch + XAzimuth);

    % if X acceleration initially recorded is negative, convert to negative
    if Accelerations(a,1) < 0
        FinalAccelerations(a,1) = FinalAccelerations(a,1)*-1;
    end


%-----------------------------------------------------------------------%


                     %% Y ACCELERATION CALCULATIONS


    if Orientations(a,2) > 0 % If Pitch theta > 0
        % if theta > 0 add 9.81sind(theta) to remove gravity acceleration component
        YMeasure = Accelerations(a,2)-.054-9.81*sind(Orientations(a,2));

    else
        % if theta > 0 add 9.81sind(theta) to remove gravity acceleration component
        YMeasure = Accelerations(a,2)-.054+9.81*sind(Orientations(a,2));  
    end 

    YRoll = (YMeasure/cosd(Orientations(a,3)))^2;
    YAzimuth = (YMeasure/cosd(Orientations(a,1)))^2;
    FinalAccelerations(a,2) = sqrt(YRoll+YAzimuth);
 
    % if Y acceleration intially recorded is negative, convert to negative
    if Accelerations(a,2) < 0
        FinalAccelerations(a,2) = FinalAccelerations(a,2)*-1;
    end
% FinalAccelerations(a,3) = 3 % Z acceleration, not calculating Z pos off
% accelerometer data


%-----------------------------------------------------------------------%  


                        %% Calculating Velocities

    XSpeed = XSpeed + FinalAccelerations(a,1)*.01;
    YSpeed = YSpeed + FinalAccelerations(a,2)*.01;

                        %% Calculating Position

    XPos = XPos+ XSpeed*.01;
    YPos = YPos + YSpeed*.01;


    numRows = numRows - 1;
 end
 
 "X = " + XPos + " Meters"
 "Y = " + YPos + " Meters"

