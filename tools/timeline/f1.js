/* (C)2014 @animateddata */
"use strict";
var animdata = animdata || {};
animdata.tooltip = function() {
    function a(a) {
        var b = ["th", "st", "nd", "rd", "th", "th", "th", "th", "th", "th", "th", "th", "th", "th", "th", "th", "th", "th", "th", "th", "th", "st", "nd", "rd", "th", "th", "th", "th", "th", "th", "th"];
        return a + b[a % 30]
    }
    var b = {
            uiState: {
                visible: !1,
                data: {}
            }
        },
        c = {
            tooltip: d3.select("div.tooltip"),
            container: d3.select("div.wrapper")
        };
    return b.init = function() {}, b.update = function() {
        if (!b.uiState.visible) return void c.tooltip.transition().style("opacity", 0).each("end", function() {
            d3.select(this).style("display", "none")
        });
        c.tooltip.style("display", "block").transition().style("opacity", .9);
        var d = b.uiState.data,
            e = c.container[0][0],
            f = d3.mouse(e),
            g = f[0] + 10;
        f[0] > .5 * screen.width && (g -= 240);
        var h = f[1] + 10;
        f[1] > .5 * screen.height && (h -= 110), c.tooltip.style("left", g + "px").style("top", h + "px"), c.tooltip.select(".driver").text(d.driver), c.tooltip.select(".date").text(d.date), c.tooltip.select(".race").text(d.race), c.tooltip.select(".position").text(1 === d.position ? "Winner" : 0 === d.position ? "DNF" : a(d.position))
    }, b
}();
var animdata = animdata || {};
animdata.log = function() {
        var a, b = {};
        return b.init = function() {
            a = Date.now()
        }, b.write = function(a) {
            return
        }, b
    }(),
    function() {
        for (var a = 0, b = ["ms", "moz", "webkit", "o"], c = 0; c < b.length && !window.requestAnimationFrame; ++c) window.requestAnimationFrame = window[b[c] + "RequestAnimationFrame"], window.cancelAnimationFrame = window[b[c] + "CancelAnimationFrame"] || window[b[c] + "CancelRequestAnimationFrame"];
        window.requestAnimationFrame || (window.requestAnimationFrame = function(b) {
            var c = (new Date).getTime(),
                d = Math.max(0, 16 - (c - a)),
                e = window.setTimeout(function() {
                    b(c + d)
                }, d);
            return a = c + d, e
        }), window.cancelAnimationFrame || (window.cancelAnimationFrame = function(a) {
            clearTimeout(a)
        })
    }();
var animdata = animdata || {};
animdata.scroll = function() {
    var a = {};
    a.moveTo = function(a, b, c) {
        window.scrollTo(a, b), c()
    }, a.scrollTo = function(a, c) {
        function d(a, b) {
            return b > a ? a : b
        }

        function e() {
            var b = Date.now(),
                h = d(1, (b - g) / j),
                k = f(h);
            window.scrollTo(0, k * (a - i) + i), animdata.controller.fixHeader(), 1 > h ? requestAnimationFrame(e) : c && c()
        }
        animdata.log.write("start scroll");
        var f = b.easeInOutCubic,
            g = Date.now(),
            h = document.documentElement.scrollTop ? document.documentElement : document.body,
            i = h.scrollTop;
        if (i === a) return c(), void animdata.log.write("returning");
        var j = .5 * Math.abs(a - i);
        1500 > j && (j = 1500), j > 3e3 && (j = 3e3), animdata.log.write("reqanimframe"), requestAnimationFrame(e), animdata.log.write("done reqanimframe")
    };
    var b = {
        easeInOutCubic: function(a) {
            return .5 > a ? 4 * a * a * a : (a - 1) * (2 * a - 2) * (2 * a - 2) + 1
        }
    };
    return a
}();
var animdata = animdata || {};
animdata.chart = function() {
    function a(a, b) {
        var c = a.name,
            d = b.name;
        return d > c ? -1 : c > d ? 1 : 0
    }

    function b(a, b) {
        var c = a.dob,
            d = b.dob;
        return c > d ? -1 : d > c ? 1 : 0
    }

    function c(a, b) {
        var c = a.numRaces,
            d = b.numRaces;
        return c > d ? -1 : d > c ? 1 : 0
    }

    function d(a, b) {
        var c = a.numWins,
            d = b.numWins;
        return c > d ? -1 : d > c ? 1 : 0
    }

    function e(a, b) {
        var c = a.numPodiums,
            d = b.numPodiums;
        return c > d ? -1 : d > c ? 1 : 0
    }

    function f(a, b) {
        var c = a.startAge,
            d = b.startAge;
        return d > c ? -1 : c > d ? 1 : 0
    }

    function g(a) {
        return j(a.age)
    }

    function h(a) {
        return j(new Date(animdata.data.races[a.id].date))
    }

    function i(a, b) {
        return j(b)
    }
    var j, k = {
            uiState: {
                xType: "age",
                floatDriverLabels: !0,
                driverOrder: "dob"
            }
        },
        l = {},
        m = {
            age: d3.scale.linear().domain([18, 60]).range([0, 1e3]),
            date: d3.time.scale().domain([new Date("1950-01-01"), new Date("2015-01-02")]).range([0, 1e3]),
            histogram: d3.scale.linear().domain([0, 350]).range([0, 1e3])
        },
        n = d3.svg.axis().orient("top").tickSize(-16518),
        o = {
            headerXAxis: d3.select("div.x.axis"),
            svg: d3.select("svg"),
            chartContainer: d3.select("svg g.chart-container g.drivers"),
            xAxis: d3.select("svg g.chart-container g.x.axis"),
            drivers: null
        };
    return k.init = function(a) {
        l = a
    }, k.updateDrivers = function(a) {
        animdata.log.write("about to append drivers");
        var b = o.chartContainer.selectAll("g.driver").data(a, function(a) {
                return a.name
            }),
            c = b.enter().append("g").classed("driver", !0).attr("id", function(a) {
                return a.name.replace(" ", "").toLowerCase()
            }).attr("transform", function(a, b) {
                return "translate(" + [0, 20 * b] + ")"
            });
        c.append("line").attr({
            y1: 17,
            y2: 17,
            x1: 100,
            x2: 1100
        }), c.append("text").text(function(a) {
            return a.name
        }).attr("y", 12), animdata.log.write("drivers appended"), c.append("g").classed("results", !0).attr("transform", "translate(100, 0)").selectAll("rect").data(function(a) {
            return a.races
        }).enter().append("rect").attr("height", 14).style("fill", function(a) {
            return l.colors[a.pos]
        }).on("mouseover", function(a) {
            var b = d3.select(this.parentNode.parentNode).datum();
            l.tooltip.uiState.visible = !0;
            var c = animdata.data.races[a.id],
                d = {
                    driver: b.name,
                    race: c.name,
                    date: c.date,
                    position: a.pos,
                    age: Math.floor(a.age, 0)
                };
            l.tooltip.uiState.data = d, l.tooltip.update()
        }).on("mousemove", function() {
            l.tooltip.uiState.visible = !0, l.tooltip.update()
        }).on("mouseout", function() {
            l.tooltip.uiState.visible = !1, l.tooltip.update()
        }), animdata.log.write("Races appended"), o.drivers = o.chartContainer.selectAll("g.driver"), k.updateX(), animdata.log.write("x updated")
    }, k.updateY = function() {
        var g = {
            name: a,
            dob: b,
            races: c,
            wins: d,
            podiums: e,
            startAge: f
        };
        o.drivers.sort(g[k.uiState.driverOrder]).attr("transform", function(a, b) {
            return "translate(" + [0, 20 * b] + ")"
        })
    }, k.updateX = function() {
        animdata.log.write("start updateX");
        var a;
        j = m[k.uiState.xType].range([0, k.uiState.chartWidth]), "age" === k.uiState.xType ? a = g : "date" === k.uiState.xType ? a = h : "histogram" === k.uiState.xType && (a = i), o.drivers.selectAll("rect").attr("width", k.uiState.rectWidth).attr("x", a), animdata.log.write("updated rect positions"), o.drivers.each(function(b) {
            var b = b.races[0];
            d3.select(this).select("text").attr("x", k.uiState.floatDriverLabels ? a(b, 0) + 95 : 95)
        }), o.drivers.selectAll("line").attr("x2", k.uiState.chartWidth + 100);
        var b = k.uiState.chartWidth > 1200 && "histogram" !== k.uiState.xType ? 60 : 10;
        n.scale(j).ticks(b), o.xAxis.transition().call(n);
        var c = j.ticks(b),
            d = j.tickFormat();
        "date" === k.uiState.xType && (d = function(a) {
            return a.getFullYear()
        });
        var e = o.headerXAxis.selectAll("div.tick").data(c);
        e.enter().append("div").classed("tick", !0), e.exit().remove();
        var f = "age" === k.uiState.xType ? "yrs" : "";
        e.style("left", function(a) {
            return j(a) - 51 + "px"
        }).text(function(a) {
            return d(a) + f
        })
    }, k.highlightDriver = function(a) {
        o.drivers.classed("fade", null !== a).classed("highlight", !1), o.xAxis.classed("fade", !1), null !== a && (o.chartContainer.select("#" + a).classed("fade", !1).classed("highlight", !0), o.xAxis.classed("fade", !0))
    }, k
}();
var animdata = animdata || {};
animdata.data = function() {
    function a() {
        animdata.log.write("about to process");
        b.drivers.forEach(function(a) {
            var b = (new Date(a.dob), []);
            a.races.forEach(function(c, d) {
                b.push({
                    id: c,
                    pos: a.pos[d],
                    age: a.age[d]
                })
            }), a.races = b, delete a.pos
        }), animdata.log.write("ages computed"), b.drivers.forEach(function(a) {
            a.numRaces = a.races.length, a.numWins = a.races.filter(function(a) {
                return 1 === a.pos
            }).length, a.numPodiums = a.races.filter(function(a) {
                return 3 === a.pos || 2 === a.pos || 1 === a.pos
            }).length, a.startAge = a.races[0].age
        }), animdata.log.write("aggregates computed")
    }
    var b = {
        drivers: null,
        races: null
    };
    return b.init = function(c) {
        animdata.log.write("about to load"), d3.json("data/drivers.json", function(d, e) {
            b.drivers = e, d3.json("data/races.json", function(d, e) {
                b.races = e, animdata.log.write("data loaded"), a(), animdata.log.write("processed"), animdata.log.write("about to call ready"), c(), animdata.log.write("ready")
            })
        })
    }, b
}();
var animdata = animdata || {};
animdata.menu = function() {
    var a = {},
        b = {
            menu: d3.select(".header .menu")
        },
        c = [{
            value: "age",
            label: "How old were they?"
        }, {
            value: "date",
            label: "Timeline"
        }, {
            value: "histogram",
            label: "Race count"
        }];
    return a.init = function(a) {
        b.menu.select(".races.item select").selectAll("option").data(c).enter().append("option").attr("value", function(a) {
            return a.value
        }).text(function(a) {
            return a.label
        }), b.menu.select(".races.item select").on("change", function() {
            a.mode(this.value)
        }), b.menu.select(".wide.item input").on("change", function() {
            a.wideView(this.checked)
        })
    }, a.selectOption = function(a) {
        b.menu.selectAll(".races.item option").each(function(b) {
            this.selected = b.value === a
        })
    }, a.enable = function(a) {
        b.menu.selectAll("select").attr("disabled", a ? null : "disabled"), b.menu.selectAll("input").attr("disabled", a ? null : "disabled"), b.menu.style("opacity", a ? 1 : .3)
    }, a
}();
var animdata = animdata || {};
animdata.story = function() {
    function a() {
        f.uiState.currentStory >= h.length - 1 || (f.uiState.currentStory += 1, f.updateStory())
    }

    function b() {
        f.uiState.currentStory <= 0 || (f.uiState.currentStory -= 1, f.updateStory())
    }

    function c() {
        f.uiState.enabled = !1, f.updateStory()
    }

    function d() {
        animdata.log.write("scrollEnd start");
        var a = f.uiState.currentStory,
            b = h[a];
        e.controller.highlightDriver(b.driver), g.container.select(".text").html(b.text), g.container.select(".back.button").style("opacity", a > 0 ? 1 : .2), g.container.select(".next.button").style("opacity", a < h.length - 1 ? 1 : .2), g.container.style("left", b.position[0] + "px").style("top", b.position[1] + "px").transition().style("opacity", 1), animdata.log.write("scrollEnd end")
    }
    var e, f = {};
    f.uiState = {
        enabled: !0,
        currentStory: 0,
        xType: "age"
    };
    var g = {
            container: d3.select(".story")
        },
        h = [{
            driver: "jaimealguersuari",
            text: "Jaime Alguersuari is currently the youngest ever driver to have competed in Formula 1 making his debut aged 19. Max Verstappen is set to break this record in 2015 when he's set to race at the age of 17.",
            scrollTo: 0,
            position: [260, 160],
            xType: "age"
        }, {
            driver: "fernandoalonso",
            text: "2 times world champion Fernando Alonso was another young starter, making his debut at 19. When he won his first championship in 2005 he became the youngest ever world champion. However Sebastian Vettel now holds this record, winning the 2010 championship aged 23 yrs.",
            scrollTo: 530,
            position: [580, 805],
            xType: "age"
        }, {
            driver: "marioandretti",
            text: "As we look back in F1's history we see that drivers tended to start later. Mario Andretti was 28 when he started racing in F1 in 1968 following a successful racing career in the US.",
            scrollTo: 6900,
            position: [368, 7260],
            xType: "age"
        }, {
            driver: "arthurlegat",
            text: "The oldest debut drive was Belgian Arthur Legat who was 53 when he drove in the 1952 Belgian Grand Prix then once again the following year.",
            scrollTo: 16100,
            position: [600, 16495],
            xType: "age"
        }, {
            driver: "louischiron",
            text: "However the oldest driver in F1 was Louis Chiron who drove the 1958 Monaco Grand Prix aged 58.",
            scrollTo: 16e3,
            position: [528, 16485],
            xType: "age"
        }, {
            driver: "rubensbarrichello",
            text: "Rubens Barrichello holds the distinction of having driven in the most F1 races. His career started with Jordan in 1993 and by the end of 2011 when he retired he'd competed in 326 race weekends.",
            scrollTo: 0,
            position: [410, 100],
            xType: "histogram"
        }, {
            driver: "michaelschumacher",
            text: "Michael Schumacher has the most wins of any driver in history. He competed 308 times, winning 91 times.",
            scrollTo: 0,
            position: [410, 120],
            xType: "histogram"
        }, {
            driver: "juanfangio",
            text: "However the driver with the most wins per race is Juan Manuel Fangio who raced 58 times and won 24 times, a win ratio of 41.3%.",
            scrollTo: 2200,
            position: [340, 2500],
            xType: "histogram"
        }, {
            driver: "sebastianvettel",
            text: "Of the current drivers, Sebastian Vettel has the most wins at 39. His closest competitors are Lewis Hamilton and Fernando Alonso with 33 and 32 wins, respectively. Will they catch him?<br/>Close this box to explore further...",
            scrollTo: 600,
            position: [124, 920],
            xType: "histogram"
        }];
    return f.init = function(d) {
        e = d, g.container.select(".back.button").on("click", b), g.container.select(".next.button").on("click", a), g.container.select(".close.button").on("click", c), f.updateStory()
    }, f.updateStory = function() {
        if (animdata.log.write("updating story"), e.controller.enableMenu(!f.uiState.enabled), !f.uiState.enabled) return e.controller.highlightDriver(null), void g.container.style("display", "none");
        var a = f.uiState.currentStory,
            b = h[a];
        g.container.transition().style("opacity", 0), animdata.log.write("about to scroll"), a > 0 ? animdata.controller.scrollTo(b.scrollTo, d) : animdata.controller.moveTo(0, 0, d), animdata.log.write("done scroll"), f.uiState.xType !== b.xType && (e.controller.selectMode(b.xType), f.uiState.xType = b.xType), animdata.log.write("end updating story")
    }, f
}();
var animdata = animdata || {};
animdata.controller = function() {
    function a(a) {
        j.mode = animdata.chart.uiState.xType = a, animdata.chart.uiState.floatDriverLabels = "histogram" !== a, c(), animdata.chart.updateX();
        var d = {
            age: "dob",
            date: "dob",
            histogram: "races"
        };
        animdata.chart.uiState.driverOrder = d[a], animdata.chart.updateY(), animdata.menu.selectOption(a), b()
    }

    function b() {
        var a = [0, 0];
        j.wide && (a = m[j.mode]), animdata.scroll.moveTo(a[0], a[1], function() {})
    }

    function c() {
        var a = 1200,
            b = 2;
        if (j.wide) {
            var c = l[j.mode];
            a = c.chartWidth, b = c.rectWidth
        }
        animdata.chart.uiState.chartWidth = a, animdata.chart.uiState.rectWidth = b, d3.select("svg").attr("width", a + 130 + "px"), d3.select("div.header").style("width", a + 130 + "px"), d3.select("div.wrapper").style("width", a + 130 + "px")
    }

    function d(a) {
        j.wide = a, c(), animdata.chart.updateX(), b()
    }

    function e() {
        c(), animdata.chart.updateDrivers(animdata.data.drivers.slice(0, 80)), animdata.log.write("chart initialised"), animdata.story.init({
            controller: animdata.controller
        }), animdata.log.write("story initialised"), setTimeout(f, 1e3)
    }

    function f() {
        animdata.chart.updateDrivers(animdata.data.drivers), animdata.story.init({
            controller: animdata.controller
        })
    }
    var g = {},
        h = {
            mode: a,
            wideView: d
        },
        i = document.documentElement,
        j = {
            wide: !1,
            mode: "age"
        },
        k = ["#444", "#f03b20", "#ffeda0", "#ffeda0", "#aaa", "#aaa", "#aaa", "#aaa", "#aaa", "#aaa", "#aaa", "#444", "#444", "#444", "#444", "#444", "#444", "#444", "#444", "#444", "#444", "#444", "#444", "#444", "#444", "#444", "#444"],
        l = {
            age: {
                chartWidth: 6e3,
                rectWidth: 4
            },
            date: {
                chartWidth: 24e3,
                rectWidth: 6
            },
            histogram: {
                chartWidth: 2500,
                rectWidth: 6
            }
        },
        m = {
            age: [0, 0],
            date: [24e3, 0],
            histogram: [0, 0]
        };
    return g.fixHeader = function() {
        var a = (window.pageYOffset || i.scrollTop) - (i.clientTop || 0);
        d3.select("div.x.axis").style("top", a + "px")
    }, g.load = function() {
        window.onscroll = g.fixHeader, animdata.menu.init(h), animdata.chart.init({
            tooltip: animdata.tooltip,
            colors: k
        }), animdata.data.init(e)
    }, g.scrollTo = animdata.scroll.scrollTo, g.moveTo = animdata.scroll.moveTo, g.highlightDriver = animdata.chart.highlightDriver, g.enableMenu = animdata.menu.enable, g.selectMode = a, g
}(), animdata.controller.load();
