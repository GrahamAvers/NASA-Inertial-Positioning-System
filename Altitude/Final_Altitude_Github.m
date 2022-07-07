%% This Script Downloads elevation data using the Google Earth API (Make sure to set up an API_Key) 
%% It then filters through all of the data and removes all point not within the specified tolerance
%% of the final landing elevation
%% Make sure you have getElevationsPath.m in the same folder as this script


clearvars;
clc;
SAMPLES = 50;
API_KEY = '';   % Read https://developers.google.com/maps/documentation/elevation/get-api-key 


Coord1 = [40.59, -104.94];
Coord2 = [40.49, -105.15];
try
    load(['backup_' num2str(SAMPLES)]);
    
    % Sucess loaded but the number of samples are different or the
    % coordinates are different. Need to request elevations again.
    if length(lat_map)~=SAMPLES || lat_map(1)~=Coord1(1) || lng_map(1)~=Coord1(2) ...
            || lat_map(length(lat_map),1)~=Coord2(1) || lng_map(1,length(lng_map))~=Coord2(2)
        
        disp('Requesting Elevations to Google');
        
        lat = linspace(Coord1(1), Coord2(1), SAMPLES);  %Latitude Points
        
        % Preallocating memory for speed improvement
        elevation_map = NaN(SAMPLES, SAMPLES);
        resolution_map = NaN(SAMPLES, SAMPLES);
        lat_map = NaN(SAMPLES, SAMPLES);
        lng_map = NaN(SAMPLES, SAMPLES);
        
        % Gets the area elevations.
        for r=1:length(lat)
            [elevation_map(r,:), resolution_map(r,:), lat_map(r,:), lng_map(r,:)] = getElevationsPath(lat(r), Coord1(2), lat(r), Coord2(2), SAMPLES, 'key', API_KEY);
        end
        
        save(['backup_' num2str(SAMPLES)], 'elevation_map', 'resolution_map', 'lat_map', 'lng_map');
    end
catch
    
    disp('Requesting Elevations to Google');
    
    lat = linspace(Coord1(1), Coord2(1), SAMPLES);  %Latitude Points
    
    % Preallocating memory for speed improvement
    elevation_map = NaN(SAMPLES, SAMPLES);
    resolution_map = NaN(SAMPLES, SAMPLES);
    lat_map = NaN(SAMPLES, SAMPLES);
    lng_map = NaN(SAMPLES, SAMPLES);
    
    % Gets the area elevations.
    for r=1:length(lat)
        [elevation_map(r,:), resolution_map(r,:), lat_map(r,:), lng_map(r,:)] = getElevationsPath(lat(r), Coord1(2), lat(r), Coord2(2), SAMPLES, 'key', API_KEY);
    end
    
    save(['backup_' num2str(SAMPLES)], 'elevation_map', 'resolution_map', 'lat_map', 'lng_map');
end

%%                     Done Getting Data From Google
%% ---------------------------------------------------------------------%%
%%                          Now Filtering Data

% This Script Calculates final possible positions based off an altitude
% reading

final_elevation = 1500;
[numRows,numCols] = size(elevation_map);


for i=1:numRows
    for x=1:numCols
            if abs(elevation_map(i,x)-final_elevation)>5
                elevation_map(i,x)=0;
            end
    end
end
final_lat =[];
final_lon =[];
count=1;
georee=table;
Geometry="point";
CoordinateSystem="geographic";
numPoints=1;
for i=1:numRows
    for x=1:numCols
            if elevation_map(i,x)>0
                final_lat(1,count)=lat_map(i,x);
                final_lon(1,count)=lng_map(i,x);
                tempx = lat_map(i,x);
                tempy = lng_map(i,x);
                p=geopoint;
                p = geopoint(tempx,tempy);
                georee.Shape(count,1)=p;
                count=count+1;
            end
    end
end

%% Displaces possible elevations on mesh map
disp('Displaying Data');
% Displays the elevations on mesh map
figure('Name','Elevation');
subplot(2,1,1);
meshc(lng_map(1,:), lat_map(:,1), elevation_map);
title('Elevation profile from West Grand Junction');
xlabel('Latitude (º)');
ylabel('Longitude (º)');
zlabel('Elevation (m)');
colorbar;


%% Graph possible positions on high res satelite map
figure
title('Posible Final Positions Based Off Final Elevation');
h = geoplot(georee.Shape(:,1).Latitude,georee.Shape(:,1).Longitude,MarkerSize=20),'-o';
h.Color = 'r';
h.MarkerFaceColor = 'r';
h.Marker=".";
h.LineStyle = 'none';
geobasemap satellite
legend