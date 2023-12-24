#ifndef SERVER
#define SERVER

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <RFID.h>

void server_setup();

const char root_html[] = R"rawliteral(<!DOCTYPE html>
<html>
<style>

button {
  width: 100%;
  background-color: #ff0000;
  color: white;
  padding: 14px 50px;
  margin: 8px 0;
  border: none;
  border-radius: 4px;
  cursor: pointer;
}

button:hover {
  background-color: #b6882b;
}

div {
  border-radius: 5px;
  background-color: #f2f2f2;
  padding: 20px;
  margin: auto;
  width: fit-content;
  position: absolute;
  top: 30%;
  left: 40%;
}
html {
  background-color: #000;
}
</style>

<script type="text/javascript">
  function userRedirect(){
    let url = window.location.href  + "personnel"
    window.open(url)

  }

  function reportRedirect(){
    let url = window.location.href + "report"
    window.open(url)
  }
</script>

<body>

  <div>
    <h2 style="text-align:center">Management</h2>
    <button id="users" name="personnel" onclick="userRedirect()">personnel</button>
    <button id="report" name="report", onclick="reportRedirect()">report</button>
  </div>

</body>
</html>
)rawliteral";

const char personnel_html[] = R"rawliteral(<!DOCTYPE html>
<html>
<head>
    <style id="message">
      html {
        background-color: #000;
      }
      div {
        margin: 60px;
      }

      table#show {
        font-family: arial, sans-serif;
        width: 100%;
      }

      table#show td,
      th {
        border: 1px solid #dddddd;
        text-align: left;
        color: #ffffff;
        padding: 8px;
      }
      form {
        padding: 0px 0px 20px 0px;
        display: flex;
        /* justify-content: center; */
      }

      input[type="text"],
      select {
        width: 100%;
        padding: 6px 10px 6px 10px;
        margin: 8px 5px 0px 5px;
        display: inline-block;
        border: 1px solid #ccc;
        border-radius: 4px;
        box-sizing: border-box;
      }

      input[type="submit"] {
        width: 20%;
        background-color: #ffc400;
        color: white;
        padding: 10px 25px 10px 25px;
        margin: 8px 0px 0px 10px;
        border: none;
        border-radius: 4px;
        cursor: pointer;
      }
      input[id="no"] {
        width: 20%;
        background-color: #ff1e00;
        color: rgb(255, 251, 0);
        padding: 10px 25px 10px 25px;
        margin: 8px 0px 0px 10px;
        border: none;
        border-radius: 4px;
        cursor: pointer;
      }

      input[type="submit"]:hover {
        background-color: #b6882b;
      }

      button {
        width: 10%;
        background-color: #ffc400;
        color: white;
        padding: 14px 50px;
        margin: 8px 0;
        border: none;
        border-radius: 4px;
        cursor: pointer;
      }

      button:hover {
        background-color: #b6882b;
      }
    </style>

    <script type="text/javascript">
      let xhttp = new XMLHttpRequest();
      let xhtt = new XMLHttpRequest();
      async function getData() {

        document.getElementById("boddy").innerHTML = "<div><table id=\"show\"><tr><th>Name</th><th>UID</th></tr><tr></tr></table></div>"+
        "<div><form action=\"personneladd\" , method=\"GET\" , id=\"myForm\"><input type=\"text\" placeholder=\"User Name\" name=\"NAME\" id=\"user\" />"+
        "<input onclick=\"alert(\"Use the card of new personnel...\")\" type=\"submit\" value=\"Add personnel\"/></form><form action=\"personneldelete\" method=\"GET\" id=\"myForm\">"+
        "<input type=\"text\" placeholder=\"Name\" name=\"NAME\" id=\"delete\" /><input type=\"submit\" id=\"no\" value=\"Remove personnel\"/></form></div>";
        
        xhttp.onreadystatechange = function () {
          if (this.readyState == 4) {
            console.log("ready state reached");
            if (this.status == 200) {
              console.log("status 200");

              data = this.responseText;
              data = JSON.parse(data);

              let tb = document.getElementById("show");

              for (let i = 0; i < data.USERS.length; i++) {
                let row = tb.insertRow(-1);
                row.id = data.USERS[i].NAME.replace(" ", "");
                console.log(row.id);

                let c1 = row.insertCell(0);
                let c2 = row.insertCell(1);

                c1.innerHTML = data.USERS[i].NAME;
                c2.innerHTML = data.USERS[i].UID;
              }
            } else {
              console.log("a problem ocurred! retry", "\n", this.responseText);
            }
          }
        };
        xhttp.open("GET", "personnelinfo", true); //first field is the mthod of the request(must be get for geting information) . second field is our path which we recognize the request by that in server side . last field must set true
        xhttp.send(); //send request to server
      }
      async function security(){
        alert("Waiting for the Master Card...")
        xhtt.onreadystatechange = function () {
          if (this.readyState == 4) {
            console.log("ready state reached");
            if (this.status == 200) {
              console.log("status 200");

              data = this.responseText;
              data = JSON.parse(data);

              if(data.SECURE[0].STATUS == "true"){
                getData();
              }
              else{
                document.getElementById("boddy").innerHTML = "<H1>Authorization Failed</H1><P>Without a valid master card,access to this page cannot be granted."+
                "Please reload the page and use a valid master card when prompted.</P>"
                
                document.getElementById("message").innerHTML = "html {background-color: #FFFFFF;}";
              }
            } else {
              console.log("a problem ocurred! retry", "\n", this.responseText);
            }
          }
        };
        xhtt.open("GET", "security", true); //first field is the mthod of the request(must be get for geting information) . second field is our path which we recognize the request by that in server side . last field must set true
        xhtt.send(); //send request to server       
      }
    </script>
  </head>
  <body onload="security()" id="boddy" ></body>
</html>
)rawliteral";

const char report_html[] = R"rawliteral(
<html>
<style>
  h2{
    text-align:center; color:#FFFFFF
  }
  div {
    margin: 60px;
  }
  table#show {
    font-family: arial, sans-serif;
    width: 100%;
  }

  table#show td,
  th {
    border: 1px solid #dddddd;
    text-align: left;
    color: #ffffff;
    padding: 8px;
  }
  html {
    background-color: #000;
}
</style>

<script type="text/javascript">
  let xhtt = new XMLHttpRequest();
  async function report(){
    xhtt.onreadystatechange = function () {
      if (this.readyState == 4) {
        console.log("ready state reached");
        if (this.status == 200) {
          console.log("status 200");

          data = this.responseText;
          data = JSON.parse(data);
          let tb = document.getElementById("show");

          for (let i = 0; i < data.USERS.length; i++) {
            let row = tb.insertRow(-1);
            row.id = data.USERS[i].NAME.replace(" ", "");
            console.log(row.id);

            let c1 = row.insertCell(0);
            let c2 = row.insertCell(1);

            c1.innerHTML = data.USERS[i].NAME;
            c2.innerHTML = data.USERS[i].UID;
          }
        } else {
          console.log("a problem ocurred! retry", "\n", this.responseText);
        }
      }
    };
    xhtt.open("GET", "reportinfo", true); //first field is the mthod of the request(must be get for geting information) . second field is our path which we recognize the request by that in server side . last field must set true
    xhtt.send(); //send request to server       
  }
</script>

<body onload="report()">
  <h2>Present members:</h2>
  <div>
    <table id="show">
      <tr>
        <th>Name</th>
        <th>UID</th>
      </tr>
    </table>
  </div>

</body>
</html>
)rawliteral";

#if !defined(SERV)
  extern bool rfid_flag;
  extern AsyncWebServer server;
#endif
#endif