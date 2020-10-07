<?php include 'connection.php';
?>



<!DOCTYPE html> 
<html> 
  <head> 
    <style> 
      #map { 
        height: 400px; 
        width: 100%; 
       } 
    </style> 
  </head> 
  <body> 
    <h3>GFG Google Maps Demo</h3> 
	<?php 
	$sql = "SELECT * from podaci;";
	$result = mysqli_query($conn, $sql); 
	$resulCheck = mysqli_num_rows($result);
	if($resulCheck>0)		
		{
			while($row = mysqli_fetch_assoc($result))
			{
				$str_lat1 = $row['Latituda'];
				$lat1 = floatval($str_lat1);
				$str_lng1 = $row['Longituda'];				
				$lng1 = floatval($str_lng1);
				
			}
		}
	
	?>
    <div id="map"></div> 
    <script> 
	var lat1 = <?= json_encode($lat1) ?>;
	var lng1 = <?= json_encode($lng1) ?>;
	document.write(lat1,lng1);
      function initMap() { 
        var sarajevo = {lat: 43.856430, lng: 18.413029}; 
		var sar_marker = {lat: lat1, lng: lng1 };
        var map = new google.maps.Map(document.getElementById('map'), { 
          zoom: 15, 
          center: sarajevo 
        }); 
        var marker = new google.maps.Marker({ 
          position: sar_marker, 
          map: map 
        });  
      } 
    </script> 
    <script async defer 
    src= 
"https://maps.googleapis.com/maps/api/js?key=AIzaSyCn2S4NaNxxUGyC7GL-kf_87ifFFMC8nyQ&callback=initMap"> 
    </script> 
  </body> 
</html> 