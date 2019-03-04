<?php

namespace Todaymade\Daux\Extension;

use League\CommonMark\Environment;
use League\CommonMark;
use Todaymade\Daux\Tree\Root;

class APIDocLinkParser extends CommonMark\Inline\Parser\AbstractInlineParser
{
    private $memberLookup;

    public function __construct($memberLookup) {
        $this->memberLookup = $memberLookup;
    }

    public function getCharacters()
    {
        return ['@'];
    }

    public function parse(CommonMark\InlineParserContext $inlineContext)
    {
        $cursor = $inlineContext->getCursor();

        // The @ symbol must not have any other characters immediately prior
        $previousChar = $cursor->peek(-1);
        if ($previousChar !== null && $previousChar !== ' ' && $previousChar !== '\t')
            return false;

        // Save the cursor state in case we need to rewind and bail
        $previousState = $cursor->saveState();

        // Advance past the @ symbol
        $cursor->advance();

        // Parse the type name
        $typeName = $cursor->match('/^[A-Za-z0-9_:]*/');
        if (empty($typeName)) {
            // Regex failed to match
            $cursor->restoreState($previousState);
            return false;
        }

        // Add bs:: prefix if not specified, and not in some other namespace
        if(substr($typeName, 0, 4) != 'bs::')
        {
            $hasNs = strpos($typeName, '::');

            if($hasNs === FALSE)
                $typeName = 'bs::' . $typeName;
            else
            {
                $ns = substr($typeName, 0, $hasNs);
                if($ns === 'ct')
                    $typeName = 'bs::' . $typeName;
            }
        }

        if(!array_key_exists($typeName, $this->memberLookup)) {
            \Todaymade\Daux\Daux::writeln("Unable to find documentation for type link '$typeName'.");

            $cursor->restoreState($previousState);
            return false;
        }

        $memberInfo = $this->memberLookup[$typeName];

        $linkUrl = "file://" . __DIR__ . DIRECTORY_SEPARATOR . '..' . DIRECTORY_SEPARATOR . 'api' . DIRECTORY_SEPARATOR .
            $memberInfo->path . ".html";
        if($memberInfo->header !== "")
            $linkUrl .= '#' . $memberInfo->header;

        $readableName = str_replace("bs::","", $typeName);
        $inlineContext->getContainer()->appendChild(new CommonMark\Inline\Element\Link($linkUrl, $readableName));
        return true;
    }
}

class APIDocMemberInfo
{
    public $name = "";
    public $localName = "";
    public $path = "";
    public $header = "";
}

class APIDocCompoundInfo
{
    public $name = "";
    public $refid = "";
    public $members = array();
    public $bases = array();
}

class Processor extends \Todaymade\Daux\Processor
{
    public function extendCommonMarkEnvironment(Environment $environment)
    {
        $xmlRoot = __DIR__ . DIRECTORY_SEPARATOR . '..' . DIRECTORY_SEPARATOR . 'xml' . DIRECTORY_SEPARATOR;
        $xmlPath = $xmlRoot . 'index.xml';
        $xml = simplexml_load_file($xmlPath);

        if($xml === FALSE)
            throw new \RuntimeException("Unable to parse index.xml at path '$xmlPath'");

        $memberLookup = array();
        $enumLookup = array();
        $compoundLookup = array();

        foreach($xml->compound as $compound)
        {
            $compoundKind = $compound['kind'];

            switch($compoundKind)
            {
                case "class":
                case "struct":
                case "union":
                case "namespace":
                    $compoundInfo = null;
                    $compoundParentName = $compound->name;

                    if($compoundKind != "namespace") {
                        $compoundMemberInfo = new APIDocMemberInfo();
                        $compoundMemberInfo->name = $compoundParentName;
                        $compoundMemberInfo->localName = $compoundParentName;
                        $compoundMemberInfo->path = $compound["refid"];
                        $compoundMemberInfo->header = "";

                        $memberLookup["$compound->name"] = $compoundMemberInfo;

                        $compoundInfo = new APIDocCompoundInfo();
                        $compoundInfo->name = $compoundParentName;
                        $compoundInfo->refid = $compound["refid"];

                        // Parse base class
                        $classXmlPath = $xmlRoot . $compoundMemberInfo->path . '.xml';
                        $classXml = simplexml_load_file($classXmlPath);

                        if($classXml !== FALSE)
                        {
                            foreach($classXml->compounddef->basecompoundref as $baseClass)
                                array_push($compoundInfo->bases, (string)$baseClass["refid"]);
                        }
                    }

                    foreach($compound->member as $member)
                    {
                        $memberName = $member->name;
                        $memberKind = $member['kind'];

                        $memberRefid = substr($member["refid"], strlen($member["refid"]) - 33);
                        $memberPath = substr($member["refid"], 0, -33);

                        if($memberKind == "enum")
                            $enumLookup["$memberRefid"] = $memberName;

                        if($memberKind == "enumvalue")
                        {
                            $parentRefid = substr($memberPath, strlen($memberPath) - 33);
                            $memberRefid = $parentRefid . $memberRefid;
                            $memberPath = substr($memberPath, 0, -33);

                            $memberName = $enumLookup["$parentRefid"] . '::' . $memberName;
                        }

                        $separatorPos = strrpos($memberPath, '_');
                        $memberPath = substr($memberPath, 0, $separatorPos);

                        $fullMemberName = $memberName;
                        if($compoundParentName != "")
                            $fullMemberName = $compoundParentName . '::' . $memberName;

                        switch($memberKind)
                        {
                            case "enum":
                            case "enumvalue":
                            case "function":
                            case "variable":

                            $memberInfo = new APIDocMemberInfo();
                            $memberInfo->name = $fullMemberName;
                            $memberInfo->localName = $memberName;
                            $memberInfo->path = $memberPath;
                            $memberInfo->header = $memberRefid;

                            $memberLookup["$fullMemberName"] = $memberInfo;

                            if($compoundInfo !== null)
                                array_push($compoundInfo->members, $memberInfo);
                        }
                    }

                    if($compoundInfo !== null)
                        $compoundLookup["$compoundInfo->refid"] = $compoundInfo;

            }
        }

        // Resolve base classes
        foreach($compoundLookup as $compoundName => $compound)
        {
            echo($compound->name . "\r\n");
            foreach($compound->bases as $base)
            {
                if(!array_key_exists("$base", $compoundLookup))
                    continue;

                $baseCompound = $compoundLookup["$base"];
                foreach($baseCompound->members as $baseMember)
                {
                    $memberInfo = new APIDocMemberInfo();
                    $memberInfo->name = $compound->name . '::' . $baseMember->localName;
                    $memberInfo->localName = $baseMember->localName;
                    $memberInfo->path = $baseMember->path;
                    $memberInfo->header = $baseMember->header;

                    $memberLookup["$memberInfo->name"] = $memberInfo;

                    if($compound->name === "bs::Application")
                        echo("Adding member $memberInfo->name to $compoundName from $baseCompound->name");
                }
            }
        }

        $environment->addInlineParser(new APIDocLinkParser($memberLookup));
    }
}
