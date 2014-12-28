

function Login_Page_Loaded(){

}

function Login_Return(){
    if (xmlhttp.readyState==4){
	Login_Form.style.display = "none";
	if(xmlhttp.status==200){
	    stuff.innerHTML = "Welcome "+uname;
	}
	else if(xmlhttp.status==500||xmlhttp.status==400){
	    stuff.innerHTML = stuff.innerHTML+" : "+xmlhttp.responseText;
	}
	else{
	    stuff.innerHTML="Faute d'erreur! Grave! Grave!!";
	}
    }
}


function Login(){
    Login_Form = window.document.getElementById("LoginForm");
    stuff = window.document.getElementById("stuff");
    uname = window.document.getElementById("uname").value;
    pass = window.document.getElementById("pass").value;
    var url = "/api/login/?user="+uname+"&pass="+pass;
    stuff.innerHTML = url;
    xmlhttp=new XMLHttpRequest();
    xmlhttp.onreadystatechange=Login_Return;
    xmlhttp.open("GET",url,true);
    xmlhttp.send();

}

