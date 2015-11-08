#include "BsD3D9EmulatedParamBlocks.h"
#include <regex>

namespace BansheeEngine
{
	String D3D9EmulatedParamBlockParser::parse(const String& gpuProgSource, Vector<D3D9EmulatedParamBlock>& paramBlocks)
	{
		static std::regex paramBlockRegex("BS_PARAM_BLOCK\\s*(.*\\})");
		static std::regex blockNameRegex("([^\\s\\{]*)");
		static std::regex paramNameRegex("(?:\\{ *([^\\, ]*))|(?:\\, *([^\\, \\}]*))");
		static std::regex replaceRegex("BS_PARAM_BLOCK\\s*(.*\\}\\n?)");

		std::sregex_iterator paramBlockIter(gpuProgSource.begin(), gpuProgSource.end(), paramBlockRegex);
		std::sregex_iterator iterEnd;

		while (paramBlockIter != iterEnd)
		{
			std::string stdString = (*paramBlockIter)[1];
			String paramBlockString = String(stdString.begin(), stdString.end());

			paramBlocks.push_back(D3D9EmulatedParamBlock());
			D3D9EmulatedParamBlock& block = paramBlocks.back();

			std::smatch nameMatch;
			if (std::regex_search(paramBlockString, nameMatch, blockNameRegex))
			{
				stdString = nameMatch[1];
				block.blockName = String(stdString.begin(), stdString.end());
			}

			std::sregex_iterator paramNameIter(paramBlockString.begin(), paramBlockString.end(), paramNameRegex);
			while (paramNameIter != iterEnd)
			{
				if((*paramNameIter)[1].matched)
					stdString = (*paramNameIter)[1];
				else
					stdString = (*paramNameIter)[2];

				block.paramNames.push_back(String(stdString.begin(), stdString.end()));
				++paramNameIter;
			}

			++paramBlockIter;
		}

		// Return string without param block definitions
		return std::regex_replace(gpuProgSource, replaceRegex, "");
	}
}