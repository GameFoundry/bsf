namespace BansheeEngine
{
	/**
	 * @brief	Helper class used for string formatting operations.
	 */
	class StringFormat
	{
	private:
		/**
		 * @brief	Data structure used during string formatting. It holds
		 *			information about parameter identifiers to replace with
		 *			actual parameters.
		 */
		struct FormatParamRange
		{
			FormatParamRange() { }
			FormatParamRange(UINT32 start, UINT32 identifierSize, UINT32 paramIdx)
				:start(start), identifierSize(identifierSize), paramIdx(paramIdx)
			{ }

			UINT32 start;
			UINT32 identifierSize;
			UINT32 paramIdx;
		};

		/**
		 * @brief	Structure that holds value of a parameter during string
		 *			formatting.
		 */
		template<class T>
		struct ParamData
		{
			T* buffer;
			UINT32 size;
		};

	public:
		/**
		 * @brief	Formats the provided string by replacing the identifiers with the provided parameters.
		 *			The identifiers are represented like "{0}, {1}" in the source string, where the number
		 *			represents the position of the parameter that will be used for replacing the identifier.
		 *			
		 * @note	You may use "\" to escape identifier brackets.
		 *			Maximum identifier number is 19 (for a total of 20 unique identifiers. e.g. {20} won't be recognized as an identifier).
		 *			Total number of parameters that can be referenced is 200.
		 */
		template<class T, class... Args>
		static BasicString<T> format(const T* source, Args&& ...args)
		{
			UINT32 strLength = getLength(source);

			ParamData<T> parameters[MAX_PARAMS];
			memset(parameters, 0, sizeof(parameters));
			getParams<T>(parameters, 0, std::forward<Args>(args)...);

			T bracketChars[MAX_IDENTIFIER_SIZE + 1];
			UINT32 bracketWriteIdx = 0;

			FormatParamRange paramRanges[MAX_PARAM_REFERENCES];
			memset(paramRanges, 0, sizeof(paramRanges));
			UINT32 paramRangeWriteIdx = 0;

			// Determine parameter positions
			INT32 lastBracket = -1;
			bool escaped = false;
			UINT32 charWriteIdx = 0;
			for (UINT32 i = 0; i < strLength; i++)
			{
				if (source[i] == '\\' && !escaped && paramRangeWriteIdx < MAX_PARAM_REFERENCES)
				{
					escaped = true;
					paramRanges[paramRangeWriteIdx++] = FormatParamRange(charWriteIdx, 1, 0);
					continue;
				}

				if (lastBracket == -1)
				{
					// If current char is non-escaped opening bracket start parameter definition
					if (source[i] == '{' && !escaped)
						lastBracket = i;
					else
						charWriteIdx++;
				}
				else
				{
					if (isdigit(source[i]) && bracketWriteIdx < MAX_IDENTIFIER_SIZE)
						bracketChars[bracketWriteIdx++] = source[i];
					else
					{
						// If current char is non-escaped closing bracket end parameter definition
						UINT32 numParamChars = bracketWriteIdx;
						bool processedBracket = false;
						if (source[i] == '}' && numParamChars > 0 && !escaped)
						{
							bracketChars[bracketWriteIdx] = '\0';
							UINT32 paramIdx = strToInt(bracketChars);
							if (paramIdx < MAX_PARAMS && paramRangeWriteIdx < MAX_PARAM_REFERENCES) // Check if exceeded maximum parameter limit
							{
								charWriteIdx += parameters[paramIdx].size;
								paramRanges[paramRangeWriteIdx++] = FormatParamRange(charWriteIdx, numParamChars + 2, paramIdx);

								processedBracket = true;
							}
						}

						if (!processedBracket)
						{
							// Last bracket wasn't really a parameter
							for (UINT32 j = lastBracket; j <= i; j++)
								charWriteIdx++;
						}

						lastBracket = -1;
						bracketWriteIdx = 0;
					}
				}

				escaped = false;
			}

			// Copy the clean string into output buffer
			UINT32 finalStringSize = charWriteIdx;

			T* outputBuffer = (T*)bs_alloc(finalStringSize * sizeof(T));
			UINT32 copySourceIdx = 0;
			UINT32 copyDestIdx = 0;
			for (UINT32 i = 0; i < paramRangeWriteIdx; i++)
			{
				const FormatParamRange& rangeInfo = paramRanges[i];
				UINT32 copySize = rangeInfo.start - copyDestIdx;
				UINT32 paramSize = parameters[rangeInfo.paramIdx].size;

				memcpy(outputBuffer + copyDestIdx, source + copySourceIdx, copySize);
				copySourceIdx += copySize + rangeInfo.identifierSize;
				copyDestIdx += copySize;

				memcpy(outputBuffer + copyDestIdx, parameters[rangeInfo.paramIdx].buffer, paramSize);
				copyDestIdx += paramSize;
			}

			memcpy(outputBuffer + copyDestIdx, source + copySourceIdx, finalStringSize - copyDestIdx);

			BasicString<T> outputStr(outputBuffer, finalStringSize);
			bs_free(outputBuffer);

			for (UINT32 i = 0; i < MAX_PARAMS; i++)
			{
				if (parameters[i].buffer != nullptr)
					bs_free(parameters[i].buffer);
			}

			return outputStr;
		}

	private:
		/**
		 * @brief	Set of methods that can be specialized so we have a generalized way for retrieving length
		 *			of strings of different types.
		 */
		static UINT32 getLength(const char* source) { return (UINT32)strlen(source); }

		/**
		 * @brief	Set of methods that can be specialized so we have a generalized way for retrieving length
		 *			of strings of different types.
		 */
		static UINT32 getLength(const wchar_t* source) { return (UINT32)wcslen(source); }

		/**
		 * @brief	Parses the string and returns an integer value extracted from string characters.
		 */
		static UINT32 strToInt(const char* buffer)
		{
			return (UINT32)strtoul(buffer, nullptr, 10);
		}

		/**
		 * @brief	Parses the string and returns an integer value extracted from string characters.
		 */
		static UINT32 strToInt(const wchar_t* buffer)
		{
			return (UINT32)wcstoul(buffer, nullptr, 10);
		}

		/**
		 * @brief	Converts all the provided parameters into string representations and populates the provided
		 *			"parameter" array.
		 */
		template<class P, class... Args>
		static void getParams(ParamData<char>* parameters, UINT32 idx, P&& param, Args&& ...args)
		{
			if (idx >= MAX_PARAMS)
				return;

			std::basic_string<char> sourceParam = std::to_string(param);
			parameters[idx].buffer = (char*)bs_alloc(sourceParam.size() * sizeof(char));
			parameters[idx].size = (UINT32)sourceParam.size();

			sourceParam.copy(parameters[idx].buffer, parameters[idx].size, 0);
			
			getParams(parameters, idx + 1, std::forward<Args>(args)...);
		}

		/**
		 * @brief	Converts all the provided parameters into string representations and populates the provided
		 *			"parameter" array.
		 */
		template<class P, class... Args>
		static void getParams(ParamData<wchar_t>* parameters, UINT32 idx, P&& param, Args&& ...args)
		{
			if (idx >= MAX_PARAMS)
				return;

			std::basic_string<wchar_t> sourceParam = std::to_wstring(param);
			parameters[idx].buffer = (wchar_t*)bs_alloc(sourceParam.size() * sizeof(wchar_t));
			parameters[idx].size = (UINT32)sourceParam.size();
			
			sourceParam.copy(parameters[idx].buffer, parameters[idx].size, 0);

			getParams(parameters, idx + 1, std::forward<Args>(args)...);
		}

		/**
		 * @brief	Helper method for parameter size calculation. Used as a stopping point in template recursion.
		 */
		template<class T>
		static void getParams(ParamData<T>* parameters, UINT32 idx)
		{
			// Do nothing
		}

		static const UINT32 MAX_PARAMS = 20;
		static const UINT32 MAX_IDENTIFIER_SIZE = 2;
		static const UINT32 MAX_PARAM_REFERENCES = 200;
	};
}