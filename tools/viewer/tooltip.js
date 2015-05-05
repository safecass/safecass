//------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2012-2014 Min Yang Jung and Peter Kazanzides
//
//------------------------------------------------------------------------
//
// Created on   : Jul 23, 2014
// Last revision: Jul 23, 2014
// Author       : Min Yang Jung (myj@jhu.edu)
// Github       : https://github.com/minyang/casros
//
// This is modified version of tooltip.js that implements helper functions
// for tooltip management with D3.  The original source code is at:
//
// http://bl.ocks.org/biovisualize/2973775
//
d3.helper = {};

d3.helper.tooltip = function(){
    var ox = 185, oy = 30;
    var tooltipDiv;
    var bodyNode = d3.select('body').node();
    var attrs = {};
    var text = '';
    var styles = {};

    function tooltip(selection){

        selection.on('mouseover.tooltip', function(pD, pI){
            var name, value;
            // Clean up lost tooltips
            d3.select('body').selectAll('div.tooltip').remove();
            // Append tooltip
            tooltipDiv = d3.select('body').append('div');
            tooltipDiv.attr(attrs);
            var absoluteMousePos = d3.mouse(bodyNode);
            tooltipDiv.style({
                left: (absoluteMousePos[0] + ox)+'px',
                top: (absoluteMousePos[1] + oy)+'px',
                position: 'absolute',
                'z-index': 1001,
                'border': '1.5px solid black',
                'border-radius': '5px',
                'box-shadow': '2px 2px 2px #888888',
                'background-color': 'white',
                'padding': '5px 10px 5px 10px',
                'font-family': 'Myriad Web',
                'font-size': '13px'
            });
            // Add text using the accessor function, Crop text arbitrarily
            tooltipDiv.style('width', function(d, i){ return (text(pD, pI).length > 80) ? '300px' : null; })
                .html(function(d, i){return text(pD, pI);});
        })
        .on('mousemove.tooltip', function(pD, pI){
            // Move tooltip
            var absoluteMousePos = d3.mouse(bodyNode);
            tooltipDiv.style({
                left: (absoluteMousePos[0] + ox)+'px',
                top: (absoluteMousePos[1] + oy)+'px'
            });
            // Keep updating the text, it could change according to position
            tooltipDiv.html(function(d, i){ return text(pD, pI); });
        })
        .on('mouseout.tooltip', function(pD, pI){
            // Remove tooltip
            tooltipDiv.remove();
        });

    }

    tooltip.attr = function(_x){
        if (!arguments.length) return attrs;
        attrs = _x;
        return this;
    };

    tooltip.style = function(_x){
        if (!arguments.length) return styles;
        styles = _x;
        return this;
    };

    tooltip.text = function(_x){
        if (!arguments.length) return text;
        text = d3.functor(_x);
        return this;
    };

    return tooltip;
};
