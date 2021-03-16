#pragma once


#include "Common/CommonBase.h"
#include "Common/CanNotCreate.h"
#include "Common/GameData.h"
#include <filesystem>
#include <unordered_map>


namespace fs = std::filesystem;


namespace ta
{
	class XmlNode;
	class ObjGroup;
	class Vector;
	class MemoryBuffer;
}


namespace ta
{
	class FileLoader : public CanNotCreate
	{
	public:
		static bool getFilePathsFromDirectory(const fs::path& directoryPath, std::vector<fs::path>& output) noexcept;



		// 스레드함수로 사용할 경우 rootOutput은 힙에 할당되어서 들어와야한다. filePath는 복사되서 들어오기 때문에 어떤스레드든 상관없다.
		static bool loadXml(const fs::path filePath, XmlNode* rootOutput) noexcept;
		static bool loadObj(const fs::path filePath, ObjGroup* objGroupOutput) noexcept;

		static bool loadFileString(const fs::path& filePath, MemoryBuffer& buffer) noexcept;
		static bool saveFileString(const fs::path& filePath, const MemoryBuffer& buffer) noexcept;

	private:

		static bool loadXmlAttributeFromString(const std::string& nodeString, XmlNode* output) noexcept;


	};
}


namespace ta
{
	class XmlNode
	{
	public:
		XmlNode(void) noexcept;
		~XmlNode(void) noexcept;

		void clear(void) noexcept;

		uint32 getChildElementCount(void) const noexcept;
		XmlNode* getChildElement(const uint32 index) const noexcept;
		const std::string* getAttribute(const std::string& attributeName) const noexcept;

		bool addChildElement(XmlNode* childElement) noexcept;
		bool addAttribute(const std::string& name, const std::string& value) noexcept;

	private:
		std::vector<XmlNode*> _childElements;
		std::unordered_map<std::string, std::string> _attributes;
	};
}


namespace ta
{
	class ObjLineChunck
	{
	public:
		ObjLineChunck(void) noexcept;
		~ObjLineChunck(void) noexcept;

	public:
		std::vector<std::string> _lineDataSet;
	};
}


namespace ta
{
	class ObjGroup
	{
	public:
		typedef std::unordered_multimap<std::string, ObjLineChunck*> ObjGroupMap;

		ObjGroup(void) noexcept;
		~ObjGroup(void) noexcept;

		void clear(void) noexcept;

		std::pair<ObjGroupMap::const_iterator, ObjGroupMap::const_iterator> getObjLineChuncks(const std::string& keyName) const noexcept;
		void addObjLineChunck(const std::string& keyName, const std::vector<std::string>& chunkStrings) noexcept;
		void addObjLineChunck(const std::string& keyName, const std::string& lineChunkString) noexcept;
		void addObjLineChunck(const std::string& keyName, ObjLineChunck* chunk) noexcept;

	private:
		ObjGroupMap _objGroupMap;
	};
}
