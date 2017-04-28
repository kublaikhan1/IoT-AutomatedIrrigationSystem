var dynamicChart;
var channelsLoaded = 0;
var channelKeys = [];
channelKeys.push({
    channelNumber: 243137, //Channel ID
    name: 'SMART GARDEN',
    key: 'LC0SHCMDKXNHNUMI',
    fieldList: [{
        field: 1,
        axis: 'Lux'
    }, {
        field: 2,
        axis: '%RH'
    }, {
        field: 3,
        axis: 'C'
    },{
        field: 4,
        axis: 'W'
    }]
});
// user's timezone offset
var myOffset = new Date().getTimezoneOffset();
// converts date format from JSON
function getChartDate(d) {
    return Date.UTC(d.substring(0, 4), d.substring(5, 7) - 1, d.substring(8, 10), d.substring(11, 13), d.substring(14, 16), d.substring(17, 19)) - (myOffset * 60000);
}

//  This is where the chart is generated.
$(document).ready(function() {
    $('select').material_select();
    //Add Channel Load Menu
    var menu = document.getElementById("Channel Select");
    for (var channelIndex = 0; channelIndex < channelKeys.length; channelIndex++) // iterate through each channel
    {
        window.console && console.log('Name', channelKeys[channelIndex].name);
        var menuOption = new Option(channelKeys[channelIndex].name, channelIndex);
        menu.options.add(menuOption, channelIndex);
    }
    var last_date; // variable for the last date added to the chart
    window.console && console.log('Testing console');
    //make series numbers for each field
    var seriesCounter = 0
    for (var channelIndex = 0; channelIndex < channelKeys.length; channelIndex++) // iterate through each channel
    {
        for (var fieldIndex = 0; fieldIndex < channelKeys[channelIndex].fieldList.length; fieldIndex++) // iterate through each channel
        {
            channelKeys[channelIndex].fieldList[fieldIndex].series = seriesCounter;
            seriesCounter++;
        }
    }
    //make calls to load data from each channel into channelKeys array now
    // draw the chart when all the data arrives, later asyncronously add history
    for (var channelIndex = 0; channelIndex < channelKeys.length; channelIndex++) // iterate through each channel
    {
        channelKeys[channelIndex].loaded = false;
        loadThingSpeakChannel(channelIndex, channelKeys[channelIndex].channelNumber, channelKeys[channelIndex].key, channelKeys[channelIndex].fieldList);
    }

    function loadThingSpeakChannel(sentChannelIndex, channelNumber, key, sentFieldList) {
        var fieldList = sentFieldList;
        var channelIndex = sentChannelIndex;
        // get the Channel data with a webservice call
        $.getJSON('https://www.thingspeak.com/channels/' + channelNumber + '/feed.json?callback=?&amp;offset=0&amp;results=2500;key=' + key, function(data) {
                // if no access
                if (data == '-1') {
                    $('#chart-container').append('This channel is not public.  To embed charts, the channel must be public or a read key must be specified.');
                    window.console && console.log('Thingspeak Data Loading Error');
                }
                for (var fieldIndex = 0; fieldIndex < fieldList.length; fieldIndex++) // iterate through each field
                {
                    fieldList[fieldIndex].data = [];
                    for (var h = 0; h < data.feeds.length; h++) // iterate through each feed (data point)
                    {
                        var p = [] //new Highcharts.Point();
                        var fieldStr = "data.feeds[" + h + "].field" + fieldList[fieldIndex].field;
                        var v = eval(fieldStr);
                        p[0] = getChartDate(data.feeds[h].created_at);
                        p[1] = parseFloat(v);
                        // if a numerical value exists add it
                        if (!isNaN(parseInt(v))) {
                            fieldList[fieldIndex].data.push(p);
                        }
                    }
                    fieldList[fieldIndex].name = eval("data.channel.field" + fieldList[fieldIndex].field);
                }
                window.console && console.log('getJSON field name:', fieldList[0].name);
                channelKeys[channelIndex].fieldList = fieldList;
                channelKeys[channelIndex].loaded = true;
                channelsLoaded++;
                window.console && console.log('channels Loaded:', channelsLoaded);
                window.console && console.log('channel index:', channelIndex);
                if (channelsLoaded == channelKeys.length) {
                  Highcharts.setOptions({lang:{weekdays:["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"]}});
                    createChart();
                }
            })
            .fail(function() {
                alert('getJSON request failed! ');
            });
    }
    // create the chart when all data is loaded
    function createChart() {
        // specify the chart options
        var chartOptions = {
            chart: {
                type:'spline',
                renderTo: 'chart-container',
                zoomType: 'x',
                events: {
                    load: function() {
                        if ('true' === 'true' && (''.length < 1 && ''.length < 1 && ''.length < 1 && ''.length < 1 && ''.length < 1)) {
                            // If the update checkbox is checked, get latest data every 15 seconds and add it to the chart
                            setInterval(function() {
                                if (document.getElementById("Update").checked) {
                                    for (var channelIndex = 0; channelIndex < channelKeys.length; channelIndex++) // iterate through each channel
                                    {
                                        (function(channelIndex) {
                                            // get the data with a webservice call
                                            $.getJSON('https://www.thingspeak.com/channels/' + channelKeys[channelIndex].channelNumber + '/feed/last.json?callback=?&amp;offset=0&amp;location=false;key=' +
                                                channelKeys[channelIndex].key,
                                                function(data) {
                                                    for (var fieldIndex = 0; fieldIndex < channelKeys[channelIndex].fieldList.length; fieldIndex++) {
                                                        // if data exists
                                                        var fieldStr = "data.field" + channelKeys[channelIndex].fieldList[fieldIndex].field;
                                                        var chartSeriesIndex = channelKeys[channelIndex].fieldList[fieldIndex].series;
                                                        if (data && eval(fieldStr)) {
                                                            var p = [] //new Highcharts.Point();
                                                            var v = eval(fieldStr);
                                                            p[0] = getChartDate(data.created_at);
                                                            p[1] = parseFloat(v);
                                                            // get the last date if possible
                                                            if (dynamicChart.series[chartSeriesIndex].data.length > 0) {
                                                                last_date = dynamicChart.series[chartSeriesIndex].data[dynamicChart.series[chartSeriesIndex].data.length - 1].x;
                                                            }
                                                            var shift = false; //default for shift
                                                            // if a numerical value exists and it is a new date, add it
                                                            if (!isNaN(parseInt(v)) && (p[0] != last_date)) {
                                                                dynamicChart.series[chartSeriesIndex].addPoint(p, true, shift);
                                                            }
                                                        }
                                                    }
                                                });
                                        })(channelIndex);
                                    }
                                }
                            }, 1000);
                        }
                    }
                }
            },

            rangeSelector: {
                selected:1,
                inputEnabled: false
            },
            plotOptions: {
                line: {
                    connectNulls: true
                },
                series: {
                  animation: {
               duration: 2000
           },
                    marker: {
                        radius: 5
                        }
                }
            },
            tooltip: {
                shared: true,
                valueDecimals: 1
            },
            title: {
                text: 'RASPBERRYPI SMART GARDEN SENSOR DATA ',
                style: {
                  "fontSize":"24px",
                  "fontFamily":"Roboto"
                }
            },
            subtitle: {
                text: 'Contributors: Hamad Khawaja & Eliot Giese',
                style: {
                  "fontSize":"14px",
                  "fontFamily":"Roboto"
                }
            },
            xAxis: {
                type: 'datetime',
                ordinal: false,
                tickPixelInterval: 500,
                dateTimeLabelFormats: {
                    hour: '%l %p',
                    minute: '%l:%M %p'
                },
                title: {
                    text: 'LeftAxis'
                }
            },
            yAxis: [{
                title: {
                    text: 'Water Level [0-1]'
                },
                opposite: false,
                id: 'W'
            },{
                title: {
                    text: 'Soil Moisture'
                },
                opposite: false,
                id: 'Lux'
            }, {
                title: {
                    text: 'Humidity (%rh)'
                },
                opposite: true,
                id: '%RH'
            }, {
                labels: {
                    format: '{value}°C'
                },
                title: {
                    text: 'Temperature (°C)'
                },
                opposite: true,
                id: 'C'
            }],
            exporting: {
                enabled: true,
                csv: {
                    dateFormat: '%d/%m/%Y %H:%M:%S %p'
                }
            },
            legend: {
                enabled: true
            },
            navigator: {
                baseSeries: 0, //select which series to show in history navigator, First series is 0
                series: {
                    includeInCSVExport: false,
                    color: '#ff0000',
                    lineWidth: 3
                }
            },
            series: []
        };
        // add all Channel data to the chart
        for (var channelIndex = 0; channelIndex < channelKeys.length; channelIndex++) // iterate through each channel
        {
            for (var fieldIndex = 0; fieldIndex < channelKeys[channelIndex].fieldList.length; fieldIndex++) // add each field
            {
                window.console && console.log('Channel ' + channelIndex + ' field ' + fieldIndex);
                chartOptions.series.push({
                    data: channelKeys[channelIndex].fieldList[fieldIndex].data,
                    index: channelKeys[channelIndex].fieldList[fieldIndex].series,
                    yAxis: channelKeys[channelIndex].fieldList[fieldIndex].axis,
                    name: channelKeys[channelIndex].fieldList[fieldIndex].name
                 });
            }
        }
        chartOptions.xAxis.title.text = 'Date';
        // draw the chart
        dynamicChart = new Highcharts.StockChart(chartOptions);
        for (var channelIndex = 0; channelIndex < channelKeys.length; channelIndex++) // iterate through each channel
        {
            for (var fieldIndex = 0; fieldIndex < channelKeys[channelIndex].fieldList.length; fieldIndex++) // and each field
            {
                for (var seriesIndex = 0; seriesIndex < dynamicChart.series.length; seriesIndex++) // compare each series name
                {
                    if (dynamicChart.series[seriesIndex].name == channelKeys[channelIndex].fieldList[fieldIndex].name) {
                        channelKeys[channelIndex].fieldList[fieldIndex].series = seriesIndex;
                    }
                }
            }
        }
    }
});
