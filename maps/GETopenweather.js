var button = document.getElementsByClassName("button")[0];
var buttonR = document.getElementsByClassName("buttonRemove")[0];
var input = document.getElementsByClassName("inputValue")[0];

button.addEventListener("click", function () {
  fetch(
    "https://api.openweathermap.org/data/2.5/weather?q=" +
      input.value +
      ",fr&appid=c21a75b667d6f7abb81f118dcf8d4611&units=metric"
  )
    .then((response) => response.json())
    .then((data) => addIntoMap(data))
    .catch((err) => console.log(err));
});

buttonR.addEventListener("click", function () {
  removeFromMap(input.value);
});
