//
//See post: https://asmaloney.com/2014/01/code/creating-an-interactive-map-with-leaflet-and-openstreetmap/
var markers = [];
for (var i = 0; i < localStorage.length; i++) {
  markers.push(JSON.parse(localStorage.getItem(localStorage.key(i))));
}
var myURL = jQuery('script[src$="static_markers.js"]')
  .attr("src")
  .replace("static_markers.js", "");

var myIcon = L.icon({
  iconUrl: myURL + "images/pin24.png",
  iconRetinaUrl: myURL + "images/pin48.png",
  iconSize: [29, 24],
  iconAnchor: [9, 21],
  popupAnchor: [0, -14],
});

for (var i = 0; i < markers.length; ++i) {
  L.marker([markers[i].lat, markers[i].lng], { icon: myIcon })
    .bindPopup("<p>" + markers[i].name + " : " + markers[i].temp + "°C </p>")
    .addTo(map);
}

function addIntoMap(data) {
  var myCity = {
    name: data.name,
    temp: data.main.temp,
    lat: data.coord.lat,
    lng: data.coord.lon,
  };
  markers.push(myCity);
  L.marker([markers[markers.length - 1].lat, markers[markers.length - 1].lng], {
    icon: myIcon,
  })
    .bindPopup(
      "<p>" +
        markers[markers.length - 1].name +
        " : " +
        markers[markers.length - 1].temp +
        "°C </p>"
    )
    .addTo(map);
  localStorage[input.value] = JSON.stringify(myCity);
}

function removeFromMap(name) {
  window.localStorage.removeItem(name);
  map.eachLayer((layer) => {
    if (layer["_latlng"] != undefined) layer.remove();
  });
  markers = [];
  for (var i = 0; i < localStorage.length; i++) {
    markers.push(JSON.parse(localStorage.getItem(localStorage.key(i))));
    L.marker([markers[i].lat, markers[i].lng], { icon: myIcon })
      .bindPopup("<p>" + markers[i].name + " : " + markers[i].temp + "°C </p>")
      .addTo(map);
  }
}
