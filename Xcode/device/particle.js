/**
 * Particle.io API
 * This function interfaces with the particle.io REST server.
 * It returns a promise to simplify asynchronous handling.
 */
'use strict';

const Promise = require('bluebird');

var constants = require('../constants');
var helper      = require('../src/helper');

var Particle = require('particle-api-js');

var particle = new Particle();
//var token;

/* TODO: refactor to a common method if cookie.command == endpoingId always */
/**
 * Handle control On
 * @returns {Promise} success or fail
 */
function controlOn(event, context, config) {
    helper.log("controlOn",event);
    let topic = config.EventName+"/"+event.directive.endpoint.cookie.command;
    helper.log("controlOn topic",topic);
    let message = "100";
    let accessToken = event.directive.endpoint.scope.token;
    helper.log("controlOn token",accessToken);
        
//    try {   // This shouldn't be necessary

        return publish(topic, message, accessToken).then(function(result) { // Error is occuring here!
            helper.log("controlOn returning result",result);
            return result;
        },function(err) {
            helper.log("Publish Error", err);
        });
//    } catch(error) {
//        helper.log("Publish unhandled error", error);
//    }
}

function controlOff(event, context, config) {
    helper.log("controlOff",event);
    let topic = config.EventName+"/"+event.directive.endpoint.cookie.command;
    let message = "0";
    let accessToken = event.directive.endpoint.scope.token;
    return publish(topic, message, accessToken).then(function(result) {
        return result;
    });
}

function percentage(event, context, config) {
    let topic = config.EventName+"/"+event.directive.endpoint.endpointId;   // Different?
    let message = event.directive.payload.powerLevel;
    let accessToken = event.directive.endpoint.scope.token;
    return publish(topic, message, accessToken).then(function(result) {
        return result;
    });
}

function adjust(event, context, config) {
    let topic = config.EventName+"/"+event.directive.endpoint.endpointId;
    let message = "+"+event.directive.payload.powerLevelDelta;  //TODO: handle +n messages
    let accessToken = event.directive.endpoint.scope.token;
    return publish(topic, message, accessToken).then(function(result) {
        return result;
    });
}

function getVariable(deviceId, variableName, token) {
    let args = { deviceId: deviceId, name: variableName, auth: token};
    return particle.getVariable(args).then(function(response){
        return response.body.result;
    })
}

function callFunction(deviceId, functionName, argument, token) {
    let args = { deviceId: deviceId, name: functionName, argument: argument, auth: token};
    return particle.callFunction(args).then(function(response){
        return response.body.return_value;
    })
}

function publish(name, data, token) {
    helper.log("publish name",name);
    helper.log("publish data",data);
    helper.log("publish token",token);
//    let args = { name: name, data: data, auth: token, isPrivate: true };
    let args = { name: name, data: data, auth: token }; // New version?
    helper.log("publish args",args);
    try {   // This shouldn't be necessary
        return particle.publishEvent(args).then(function(response){
            helper.log("publish response",response);
            let result = response.body.ok;
            return result;
        }, function (err) {
            helper.log("publish error",err);
        });
    } catch(error) {
        helper.log("publishEvent exception", error);
    }
}

/**
 * getEndpoints
 * @param token
 * @returns {Promise}
 */
function getEndpoints(token) {

    // Get list of active Photons
    return particle.listDevices({ auth: token}).then(function(result) {

            helper.log("getEndpoints listDevices",result);

            let statusCode = result.statusCode; // s/b 200
            if(statusCode != 200) {
                console.log("Error listing devices: "+statusCode);
                return [];
            }

            let photonNames = result.body.filter(function(item) {
                return item.connected;
            }).map(function(item){
                return item.name;
            });

            // Read list of devices from each Photon
            // Use promises to wait until all are done'
            let endpoints = [];
            let lcNames = [];
            let promises = [];
            photonNames.forEach(function(name) {

                let p1 = getVariable(name, 'Devices', token).then(function (devices) {

                    helper.log("getEndpoints devices", devices);

                    let deviceStrings = devices.split(',');     //Split string into array of individual name:type strings
                    deviceStrings.forEach(function (item) {
                        helper.log("getEndpoints device", item);
                        
                        if(item) { // Not sure why this would be needed.
                            // Don't include duplicates
                            let lcName = item.toLocaleLowerCase();
                            if(lcNames.includes(lcName) == false) {
                                lcNames.push(lcName);
                                endpoints.push(endpointInfo(item,name));
                            } else {
                                helper.log("getEndpoints skipping duplicate",item);
                            }
                        }
                    });
                },
                function (err) {
                    console.log("Error reading Devices variable for " + name);
                });
                promises.push(p1);

/*                let p2 = getVariable(name, 'Supported', token).then(function (supported) {
                            let supportedStrings = supported.split(',');
                            supportedStrings.forEach(function (item) {
                                if(item) {
                                    endpoints.push(sceneEndpointInfo(item,name));
                                }
                            });
                        },
                        function (err) {
                            console.log("Error reading Supported variable for " + name);
                        });
                promises.push(p2);
 */
            });

            return Promise.all(promises).then(values => {
                helper.log("getEndpoints endpoints", endpoints);
                return endpoints;
            }).timeout(5000,"timeout");
        },
        function(err){
            console.log("Error listing devices. Returning appliances = "+JSON.stringify(appliances));
            return endpoints;
        });
}

// This function will create the endpoint JSON for each device or activity
// The name argument is used for the friendlyName.
// It will be lower-cased and spaces removed for the endpointId
function endpointInfo(name,controller) {
    helper.log('Device endpoint info: '+controller+':'+name);
    let dispCategory = "LIGHT";
    if(name.charAt(1)==':') {
        var devType = name.charAt(0);
        name = name.substring(2);
        if(devType=='C') {
            dispCategory = "INTERIOR_BLIND"; // Best fit for Curtain
        } else if(devType=='D') {
            dispCategory = "DOOR";
        } else if(devType=='F') {
            dispCategory = "FAN";
        } else if(devType=='L') {
            dispCategory = "LIGHT"; // redundant
        } else if(devType=='M') {
            dispCategory = "MOTION_SENSOR";
        } else if(devType=='S') {
            dispCategory = "SWITCH";
        } else if(devType=='T') {
            dispCategory = "TEMPERATURE_SENSOR";
        }
    }
    //TODO: Strip off trailing =## (current value)
    let splitValues = name.split('=');
    var value = 0;
    if(splitValues.length > 1) {
        name = splitValues[0];
        value = splitValues[1];
    }
    
    //TODO: parse @location (T:name@location=value)
    let id = name.replace(/\s/g,'').toLocaleLowerCase();    // Remove spaces. Numbers, letters, _-=#;:?@& only
    let friendlyName = name.toLocaleLowerCase();            // Name lower case to simplify compares. No special chars
    let description = name + ' RvSmartHome';  //
    var endpoint = {
        "endpointId": id,
        "friendlyName": friendlyName,
        "description": description,
        "manufacturerName": 'Ron Lisle',
        "displayCategories": [          // LIGHT, SMARTPLUG, SWITCH, CAMERA, DOOR, TEMPERATURE_SENSOR,
            dispCategory                // THERMOSTAT, SMARTLOCK, SCENE_TRIGGER, ACTIVITY_TRIGGER, OTHER
        ],
        "cookie": {                     // This can be anything we want, and will be passed back
            "name":     name,           // This is the mixed case name
            "command":  friendlyName,   // Currently using lower case with spaces
            "controller": controller    // Photon name (eg. "RearPanel")
        },
        "capabilities":[
            {
                "type": "AlexaInterface",
                "interface": "Alexa",
                "version": "3"
            },                          // TODO: use devType to determine capabilities
            {
                "type": "AlexaInterface",              // Is . needed? Doc shows both ways
                "interface": "Alexa.PowerController",
                "version": "3",
                "properties": {
                    "supported": [
                        {
                            "name": "powerState"
                        }
                    ],
                    "retrievable": true
                }
            },
            {
                "type": "AlexaInterface",
                "interface": "Alexa.PowerLevelController",
                "version": "3",
                "properties": {
                    "supported": [
                        {
                            "name": "powerLevel"
                        }
                    ],
                    "retrievable": true
                }
            }
        ]
    };
    helper.log("Device endpoint info returning", endpoint);
    return endpoint;
}

// This function will create the endpoint JSON for each supported activity (scene)
// The name argument is used for the friendlyName.
// It will be lower-cased and spaces removed for the endpointId
//function sceneEndpointInfo(name,controller) {
//    helper.log('Scene endpoint info:',name);
//    let id = name.replace(/\s/g,'').toLocaleLowerCase();    // Remove spaces. Numbers, letters, _-=#;:?@& only
//    let friendlyName = name.toLocaleLowerCase();            // Name lower case to simplify compares. No special chars
//    let description = name + ' connected via Particle.io';  //
//    var endpoint = {
//        "endpointId": id,
//        "friendlyName": friendlyName,
//        "description": description,
//        "manufacturerName": 'Ron Lisle',
//        "displayCategories": [          // LIGHT, SMARTPLUG, SWITCH, CAMERA, DOOR, TEMPERATURE_SENSOR,
//            "SCENE_TRIGGER"             // THERMOSTAT, SMARTLOCK, SCENE_TRIGGER, ACTIVITY_TRIGGER, OTHER
//        ],
//        "cookie": {                     // This can be anything we want, and will be passed back
//            "name":     name,           // This is the mixed case name
//            "command":  friendlyName,   // Currently using lower case with spaces
//            "controller": controller    // Photon name
//        },
//        "capabilities":[
//            {
//                "type": "AlexaInterface",              // Is . needed? Doc shows both ways
//                "interface": "Alexa.SceneController",
//                "version": "3",
//                "supportsDeactivation": true,   // Example shows not enclosed in "properties" like the devices code
//                "proactivelyReported": true
//            }
//        ]
//    };
//    return endpoint;
//}

/**
 * Handle Report State
 * @returns {Promise} success or fail
 */
function reportState(event, context, config) {
    let device = event.directive.endpoint.cookie.command;
    let photon = event.directive.endpoint.cookie.controller;
    let token = event.directive.endpoint.scope.token;

    // Read Devices variable
    let p1 = getVariable(photon, 'Devices', token).then(function(devices) {

        helper.log("reportState devices", devices);

        let deviceStrings = devices.split(',');
        deviceStrings.forEach(function (item) {
            // Skip over first 2 chars "T:"
            let itemEnd = item.substr(2);
            // Now split device name and value
            let stringParts = itemEnd.split('=');
            if(stringParts[0].localeCompare(device, 'en', { sensitivity: 'base' }) === 0) {
                helper.log("match",stringParts[0]);
                helper.log("returning",stringParts[1]);
                return(stringParts[1]);
            }
        });
    },
    function (err) {
        console.log("Error reading Devices variable for " + photon);
    });

//    return p1;
    // Return the value in a promise (Do we need this?)
    return p1.then(function(result) {
        return result;
    });
}


module.exports = {
    controlOn:controlOn,
    controlOff:controlOff,
    percentage:percentage,
    adjust:adjust,
    getVariable:getVariable,
    callFunction:callFunction,
    publish:publish,
    getEndpoints:getEndpoints,
    reportState:reportState
};
