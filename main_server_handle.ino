// Add a global variable to store the slider value


// HTML & CSS contents which display on web server
String HTML_main_server = R"(

<!DOCTYPE html>
<html>
<head>
<style>
.container {
  display: flex;
  justify-content: center;
  align-items: center;
  height: 100vh;
  flex-direction: column;
}
.button {
  border: none;
  color: white;
  padding: 15px 32px;
  text-align: center;
  text-decoration: none;
  display: inline-block;
  font-size: 16px;
  margin: 4px 2px;
  cursor: pointer;
  border-radius: 50px;
  width: 200px;
  height: 50px;
  transition: transform .2s;
}
.button:hover {
transform: scale(1.1);
}

.button1 {
background-color: #4CAF50;
}

.button2 {
background-color: #2196F3;
}

.button3 {
background-color: #f44336;
}
.title {
  font-size: 36px;
  animation: title-animation 1s ease-in-out forwards;
}

@keyframes title-animation {
  from {
    transform: translateY(-100%);
    opacity: 0;
  }
  to {
    transform: translateY(0);
    opacity: 1;
  }
}
</style>

</head>



<body>
<div class='container'>
  <h1 class='title'>NutriTron 0.1</h1>
  <br><br>
  <button class='button button1' onclick='updateValue(1)'>Wurzel Phase</button>
  <button class='button button2' onclick='updateValue(2)'>Blatt Phase</button>
  <button class='button button3' onclick='updateValue(3)'>Growing Phase</button>
  <br><br>
  <p id='selectedValue'></p>
</div>

<script>
  function updateValue(val) {
    fetch('/update', {method: 'POST', body: val});
    localStorage.setItem("selectedValue", val);
    document.getElementById('selectedValue').innerHTML = 'Selected value: ' + getMode(val);
  }

  function getMode(val) {
    switch (val) {
      case 1:
        return "Wurzel Phase";
      case 2:
        return "Blatt Phase";
      case 3:
        return "Growing Phase";
      default:
        return "No mode selected";
    }
  }

  document.getElementById('selectedValue').innerHTML = 'Selected value: ' + getMode(localStorage.getItem("selectedValue") || "<sliderValue>");

</script>
</body>
</html>

)";



// Handle root url (/)
void handle_root() {
main_server.send(200, "text/html", HTML_main_server + "<input type='hidden' id='sliderValue' value='" + String(sliderValue) + "'>");
}

// Handle the /update endpoint
void handle_update() {
  String request = main_server.arg("plain");
  sliderValue = request.toInt();
  main_server.send(200, "text/plain", "Slider value updated: " + String(sliderValue));
}

void setup_main_server(){
    main_server.on("/", handle_root);
  // Handle the /update endpoint for updating the slider value
  main_server.on("/update", HTTP_POST, handle_update);
  main_server.begin();
}
