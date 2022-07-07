# Altitude Script

## What it does:

This Matlab Script uses the Google Elevation API to download elevation data from anywhere you specify.

It then filters through the data and removes all data points not near the specified elevation

It then plots these points on a high resolution satelite map (Make sure you have the Matlab Mapping Toolbox)



## What you need:

Make sure you setup a Google Elevation API
Read https://developers.google.com/maps/documentation/elevation/get-api-key 

Also Make sure you have the Matlab Mapping Toolbox addon downloaded
https://www.mathworks.com/products/mapping.html

Final_Altitude_Github.m will not work without getElevationsPath.m

