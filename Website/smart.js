var dynamicChart;
var channelsLoaded = 0;
// put your ThingSpeak Channel#, Channel Name, and API keys here.
// fieldList shows which field you want to load, and which axis to display that field on,
// the 'T' Temperature left axis, or the 'O' Other right axis.
var channelKeys = [];
channelKeys.push({
    channelNumber: 243137, //Channel ID
    name: 'SMART GARDEN',
    key: '',
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
    // get the data using javascript's date object (year, month, day, hour, minute, second)
    // months in javascript start at 0, so remember to subtract 1 when specifying the month
    // offset in minutes is converted to milliseconds and subtracted so that chart's x-axis is correct
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
    //window.console && console.log('Channel Keys',channelKeys);
    // load the most recent 2500 points (fast initial load) from a ThingSpeak channel into a data[] array and return the data[] array
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
                  Highcharts.setOptions({lang:{weekdays:["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"]}}); // Hari indonesia
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
                TYPE:'spline',
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
                                                        //window.console && console.log('channelKeys:',channelKeys);
                                                        //window.console && console.log('chartSeriesIndex:',chartSeriesIndex);
                                                        //window.console && console.log('channel index:',channelIndex);
                                                        //window.console && console.log('field index:',fieldIndex);
                                                        //window.console && console.log('update series name:',dynamicChart.series[chartSeriesName].name);
                                                        //window.console && console.log('channel keys name:',channelKeys[channelIndex].fieldList[fieldIndex].name);
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
                buttons: [ {
                    count: 12,
                    type: 'hour',
                    text: '12H'
                }, {
                    count: 1,
                    type: 'day',
                    text: 'D'
                }, {
                    count: 1,
                    type: 'week',
                    text: 'W'
                }, {
                    count: 1,
                    type: 'month',
                    text: 'M'
                },  {
                    type: 'all',
                    text: 'All'
                }],
                inputEnabled: false,
                selected: 4 //Change to 4th button as default
            },
            plotOptions: {
                line: {
                    gapSize: 50,
                    connectNulls: true,


                                  //  dataLabels:{
                                  //  enabled:true
                                  //}
                },
                series: {
                    marker: {
                        radius: 5
                        //fillColor:'#000'
                        },
                    animation: true,
                    step: false,
                    turboThrehold: 1000,
                    borderWidth: 3
                }
            },
            tooltip: {
                shared: true,
                valueDecimals: 1,
                //xDateFormat:'%Y-%m-%d<br/>%H:%M:%S %p' //bug fix
                // reformat the tooltips so that local times are displayed
                //formatter: function() {
                //var d = new Date(this.x + (myOffset*60000));
                //var n = (this.point.name === undefined) ? '' : '<br/>' + this.point.name;
                //return this.series.name + ':<b>' + this.y + '</b>' + n + '<br/>' + d.toDateString() + '<br/>' + d.toTimeString().replace(/\(.*\)/, "");
                //}
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
                    includeInCSVExport: false
                }
            },
            series: []
                //series: [{data:[[getChartDate("2013-06-16T00:32:40Z"),75]]}]
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
                    //visible:false,
                    name: channelKeys[channelIndex].fieldList[fieldIndex].name
                });
            }
        }
        // set chart labels here so that decoding occurs properly
        //chartOptions.title.text = data.channel.name;
        chartOptions.xAxis.title.text = 'Date';

        // draw the chart
        dynamicChart = new Highcharts.StockChart(chartOptions);

        // update series number to account for the navigator series (The historical series at the bottom) which is the first series.
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
