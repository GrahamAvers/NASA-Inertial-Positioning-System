# Altitude Script

This Matlab Script uses the Google Elevation API to download elevation data from anywhere you specify.

Make sure you setup a Google Elevation API
Read https://developers.google.com/maps/documentation/elevation/get-api-key 

It then filters through the data and removes all data points not near the specified elevation

It then plots these points on a high resolution satelite map (Make sure you have the Matlab Mapping Toolbox)
