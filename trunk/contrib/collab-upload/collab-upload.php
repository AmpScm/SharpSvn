<?php

  if($argc != 9)
    die("Usage: upload.php <username> <password> <project> <folder> <file> <name> <status> <description>");
    
$c = curl_init();
curl_setopt($c, CURLOPT_COOKIESESSION, true);
curl_setopt($c, CURLOPT_FOLLOWLOCATION, true);
//curl_setopt($c, CURLOPT_VERBOSE, true);
curl_setopt($c, CURLOPT_NOPROGRESS, false);
curl_setopt($c, CURLOPT_RETURNTRANSFER, true);
curl_setopt($c, CURLOPT_AUTOREFERER, true);
curl_setopt($c, CURLOPT_COOKIEFILE, "");
curl_setopt($c, CURLOPT_USERAGENT, "Mozilla/8.0 (compatible; Collab-Upload.php/1.0)");
$posturl = "http://" . $argv[3] . ".open.collab.net/servlets/ProjectDocumentAdd?folderID=" .$argv[4];

echo "Logging in to collabnet...\n";
curl_setopt($c, CURLOPT_URL, $posturl);
if(!($page = curl_exec($c)))
{
  die("Login session retrieval failed");
}

if (!preg_match("/action=\"([^\"]+)\"/", $page, $r))
  die("No login action received");  
  
// Peform login
curl_setopt($c, CURLOPT_URL, $r[1]);
curl_setopt($c, CURLOPT_POST, true);
curl_setopt($c, CURLOPT_POSTFIELDS, // Must be send as x-www-encoded
  "loginID=" . urlencode($argv[1]) . 
  "&password=" . urlencode($argv[2]) . 
  "&rememverMe=&Login=Login");
       
echo "Creating session...\n";
       
if(!$r = curl_exec($c))
  die("Login request failed");
  
if(curl_getinfo($c, CURLINFO_EFFECTIVE_URL) != $posturl)
  die("Login failed");

curl_setopt($c, CURLOPT_URL, $posturl . "&action=Add%20document");
//curl_setopt($c, CURLOPT_POST, false);

$d = array(
  "name" => $argv[6],
  "status" => $argv[7],
  "description" => $argv[8],
  "initiallylocked" => "",
  "type" => "file",
  "textFormat" => "pre",
  "file" => "@" . $argv[5],
  "docUrl" => "",
  "submit" => "submit",
  "maxDepth" => "");
  
curl_setopt($c, CURLOPT_POSTFIELDS, $d);

echo "Posting " . $argv[5] . " as " . $argv[6] . " in folder " . $argv[4] . "...\n";

if(!$r = curl_exec($c))
  die("Uploading failed");
  
if(strstr($r, "errormessage"))
  die("Uplaod failed: File already exists?\n\n" . strstr($r, "<div class=\"errormessage\""));
else
  echo "Upload succeeded\n\n";
  
