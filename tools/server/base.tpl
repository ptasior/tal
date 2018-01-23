<html>
<head>
    <title>Server</title>
    <style>
    .treeSpan
    {
        color: silver;
        margin-right: 5px;
    }
    .log
    {
        width: 400px;
        float: left;
        border: 1px solid black;
        margin-right: 8px;
        margin-bottom: 8px;
    }
    .close
    {
        float: right;
        margin-right: 5px;
    }
    </style>
    <script type="text/javascript">
        function ajax(url, callback)
        {
            var xmlhttp = new XMLHttpRequest();
            xmlhttp.onreadystatechange = function(){
                if(xmlhttp.readyState != XMLHttpRequest.DONE)
                    return;

                if(xmlhttp.status != 200)
                    return;

                callback(xmlhttp.responseText);
            }

            xmlhttp.open("GET", url, true);
            xmlhttp.send();
        }

        window.onload = function(){
            setInterval(function(){
                    window.location.reload(); 
                    // ajax("/state", function(response){
                    //     if(response != state)
                    //         window.location.reload(); 
                    // });
                }, 5000);
        }
    </script>
</head>
<body>
    <div id="menu">
        <a href="/network">Network</a>
        <a href="/tree">Tree</a>
        <a href="/maintenance">Maintenance</a>
    </div>
    <div id="main" class="panel">
        {{!base}}
    </div>
</body>
</html>

