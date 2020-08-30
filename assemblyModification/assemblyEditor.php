<?php
$dbg = true;
$filename = "encryptor.s";
$callInstruction = "call";
$decryptFunctionName = "decryptRange@PLT";
$functionSizeKeyword = ".size";
$startLabel = "START_DECRYPT";
$endLabel = "END_DECRYPT";


$file = fopen( $filename, "r" );

if( $file === false ) {
	echo ( "Error in opening file" );
	exit();
}

$filesize = filesize( $filename );
$filetext = fread( $file, $filesize );

$modifiedFileText = parseFileText($filetext);
saveModifiedText($filename, $modifiedFileText);
fclose( $file );


function saveModifiedText($filename, $text) {
	$file = fopen( "MODIFIED".$filename, "w" );

	foreach ($text as $line) {
		fwrite($file, $line."\n");
	}

	fclose($file);
}

function debug($s) {
	global $dbg;

	if ($dbg === true) {
		echo $s."\n";
	}
}

function parseFileText($filetext) {
	global $startLabel;
	global $endLabel;
	$lines = explode("\n", $filetext);
	$modifiedText = array();
	$numDecryptFunctions = 1;

	for ($lineNum = 0, $len = count($lines); $lineNum < $len; $lineNum++) {
		$line = $lines[$lineNum];

		if (lineIsFunctionLabel($line)) {
			$functionName = parseFunctionName($line);

			debug("Found function ".$functionName);
			$parsingFunction = false;

			while (lineIsFunctionEnd($line, $functionName) === false) {
				if (lineIsCallToDecryptFunction($line)) {
					$parsingFunction = true;
					$functionArg2 = $lines[$lineNum - 2];
					$functionArg1 = $lines[$lineNum - 1];
					$modifiedText[$lineNum - 2] = modifyArgumentLine($functionArg2, $endLabel, $numDecryptFunctions, "$1");
					$modifiedText[$lineNum - 1] = modifyArgumentLine($functionArg1, $startLabel, $numDecryptFunctions, "$0");
					$modifiedText[$lineNum] = $line;
					++$lineNum;
					$line = $lines[$lineNum];
					$modifiedText[$lineNum] = createLabelForLine($startLabel, $numDecryptFunctions, $lines[$lineNum]);
					++$lineNum;
					$line = $lines[$lineNum];
					continue;
				}

//				debug("Within function ".$functionName." on line".$lineNum);
				$modifiedText[$lineNum] = $line;
				$line = $lines[++$lineNum];
			}

			if  ($parsingFunction) {
				$modifiedText[$lineNum - 1] = createLabelForLine($endLabel, $numDecryptFunctions, $lines[$lineNum - 1]);
				$numDecryptFunctions++;
			}

			debug("Function ended ".$functionName."      ".$line);

			$modifiedText[$lineNum] = $line;
		} else {
			$modifiedText[$lineNum] = $line;
		}
	}

	return $modifiedText;
}

function modifyArgumentLine($line, $label, $labelNum, $toReplace) {
	$output = $line;
	$output = str_replace("movl", "leal", $output);
	$output = str_replace($toReplace, ".".$label.$labelNum."(%rip)", $output);
	return $output;
}

function parseFunctionName($line) {
//	debug($line);
	return substr($line, 0, strlen($line) - 1);
}

function lineIsFunctionEnd($line, $functionName) {
	global $functionSizeKeyword;
	return substr_count($line, $functionSizeKeyword) == 1 && substr_count($line, $functionName) == 2;
}

function lineIsFunctionLabel($line) {
	$parts = str_split($line);
//	debug($line."    ".count($parts));
	if ($parts[0] != "." && count($parts) > 2 && $parts[count($parts) - 1] == ":") {
		return true;
	}

	return false;
}

function createLabelForLine($label, $labelNum, $line) {
	return ".".$label.$labelNum.":  ".$line;
}

function lineIsCallToDecryptFunction($line) {
	global $callInstruction;
	global $decryptFunctionName;

	return substr_count($line, $callInstruction) == 1 &&
                        substr_count($line, $decryptFunctionName) == 1;
}


?>
