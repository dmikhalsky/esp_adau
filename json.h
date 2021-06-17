const char root_response[] PROGMEM = R"=====(<!doctype html>
<html lang="en">

<head>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1">
	<title>My ADAU</title>
	<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/rangeslider.js/2.3.2/rangeslider.css">
	<style>

	</style>
	<script src="https://code.jquery.com/jquery-3.5.0.js"></script>
	<script type="text/javascript" src="https://cdnjs.cloudflare.com/ajax/libs/jquery.serializeJSON/3.2.1/jquery.serializejson.min.js"></script>
	
	<script src="https://cdnjs.cloudflare.com/ajax/libs/rangeslider.js/2.3.2/rangeslider.js"></script>
</head>

<style>
body {
  color: #404040;
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;
  padding: 5px;
  font: 20px Arial;
 }
legend {
  
  font-size: 20px;
  
}
input[type="number"] {
    font-size: 20px;
	padding: 5px;
	width: 70px;
	margin-top: 15px;
	border-style: none;
	text-align: right;
}
label {
	width: 70px;
	display: inline-block;
	
}
output {
	font: bold 32px Arial;
	margin: auto;
	display: inline-block;
	
}
input[type="radio"] {
    font-size: 20px;

}
fieldset{
	border-color:LightBlue;
}
.rangeslider{
	height: 300px;
	margin: auto;
}
.rangeslider__fill {
		background: LightBlue;
}
.btn {
		border: 2px solid black;
		border-radius: 40px;
		background-color: white;
		padding: 14px 28px;
		margin-top:30px;
		display: inline-block;
		font-size: 16px;
		cursor: pointer;
		border-color: #2196F3;
		color: dodgerblue
}
.btn:hover {
		background: #2196F3;
		color: white;
}
.u-right {
            float: right;
			margin-top: 0px;
			width:		45%;
			height:		350px;
			alignment:	center;
			text-align: center;
			
			
}
.u-left {
            float: left;
			margin-top: 0px;
			width:		45%;
			height:		350px;
			alignment:	center;
			text-align: center; 
			
}
</style>
<body>
	<form id="myForm">
		<div>
			<fieldset>
				<legend>Delay </legend> 
				<label for="left_delay">Left:</label>				
				<input type="number" id="left_delay"name="delay[]:number" value="0">
				<label for="right_delay" style = "margin-left: 30px">Right:</label>
				<input type="number" id="right_delay" name="delay[]:number" value="0">
				
			</fieldset>
			<br>
			<fieldset>
				<legend>Test Tone </legend>
				<input type="checkbox" name="Tone:boolean" data-unchecked-value="false" value="true" />
				<input type="number"  id="freq_input:number" name="frequency:number" value="500">Hz
				<br> 
			</fieldset>
			<br>
			<fieldset>
				<legend>LED </legend>
				<input type="radio" id="led_0" name="led:number" value="0"/> on 	
				<input type="radio" id="led_1" name="led:number" value="1" checked="checked" /> off	
				<br> 
			</fieldset>
			<br>
			<fieldset>
				<legend>Volume </legend>
				<div class="u-left">	
					<input type="range" id="volume1" name="volume[]:number" min="-4" max="4" step="1" value="0" data-orientation="vertical">
					<br>
					<output >0</output>
				</div>
				<div class="u-right">	
					<input type="range" style = "float: right" id="volume2" name="volume[]:number" min="-4" max="4" step="1" value="0" data-orientation="vertical">
					<br>
					<output >0</output>
				</div>
			
			</fieldset>
				
			<button type="button" class="btn" id="load_button" style="font: bold 14px Arial; height: 50px; width: 200px" >
				Defaults
			</button><div id="result" style="display: inline-block; left-margin: 200px"></div>
			
		</div>
		
	</form>
</body>
<script>
	
	var $sliders = $('input[type="range"]');
	var $delays = $("[name='delay[]:number']");
	var $output = $('output');
	var jsonString = "";

	
	$sliders
		.rangeslider({
		polyfill: false,
		onInit: function() {
			
		}
		})
		.on('input', function() {
		
			var index = $sliders.index(this)
			$output.eq(index).val(this.value).change();
		});
	
	function highlight(target, new_text, new_color){
		var orig_color = target.css("color");
		var orig_text = target.text();
   
    
		target.css({"color" : new_color});
		target.text(new_text);
   
		setTimeout(function(){
			target.css({"color" : orig_color});
			target.text(orig_text);
		
		}, 3000);
	}
	
	$('#load_button').click(function(e) {
		
		var pop = {"delay":[115,35],"Tone":false,"frequency":1000,"led":1,"volume":[0, 0, 9]}
		
		for (let i = 0; i < pop.delay.length; i++) {
			var x = pop.delay[i]
			var y=document.getElementsByName("delay[]:number")[i];
			y.value=x;
			//$delays.eq(i).val(x).change();
		}
		
		document.forms["myForm"]['led_'+pop.led].checked=true;
		document.forms["myForm"]["Tone:boolean"].checked=pop.Tone;
		
		document.forms["myForm"]["frequency:number"].value=pop.frequency;
		
		for (let i = 0; i < pop.volume.length; i++) {
			var x = pop.volume[i]
			$sliders.eq(i).val(x);
		}
		$sliders.change();
		
	});
	
	$('input').on('change', function() {
		var obj = $('form').serializeJSON();
		var min = parseInt($sliders.prop("min"));
		var max = parseInt($sliders.prop("max"));
		
			
		obj.volume.push(max-min+1);
		//obj.volume.push(max);
		
		var newjsonString = JSON.stringify(obj);
		
		if (newjsonString != jsonString){
			jsonString = newjsonString
			console.log(jsonString);
			
			$.ajax({
                url: '/plain',
                type: 'POST',
                contentType: 'plain',
                data: jsonString,
                error: function(){alert('error');},
				dataType : 'text'
				});
		}
		
		
	});


</script>
</html>)=====";