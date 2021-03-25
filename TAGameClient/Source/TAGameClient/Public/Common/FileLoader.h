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


		// XML =========================================================================================

		// 스레드함수로 사용할 경우 rootOutput은 힙에 할당되어서 들어와야한다. filePath는 복사되서 들어오기 때문에 어떤스레드든 상관없다.
		static bool loadXml(const fs::path filePath, XmlNode* rootOutput) noexcept;
		static bool saveXml(const fs::path filePath, const XmlNode* rootInput) noexcept;

		// =============================================================================================

		static bool loadObj(const fs::path filePath, ObjGroup* objGroupOutput) noexcept;

		static bool loadFileString(const fs::path& filePath, MemoryBuffer& buffer) noexcept;
		static bool saveFileString(const fs::path& filePath, const MemoryBuffer& buffer) noexcept;
		static bool saveFileString(const fs::path& filePath, const std::string& buffer) noexcept;

		static bool loadBinaryFile(const fs::path& filePath, MemoryBuffer& buffer) noexcept;
		static bool saveBinaryFile(const fs::path& filePath, const MemoryBuffer& buffer) noexcept;

	private:

		// XML =========================================================================================
		
		// save
		static bool makeElementString(std::vector<const XmlNode*>& elementStack, std::string& output) noexcept;
		static void makeDepthString(const uint32 depth, std::string& output) noexcept;
		static void makeAttributeString(const XmlNode* input, std::string& output) noexcept;

		// load
		static bool loadXmlNodeFromString(const std::string& nodeString, XmlNode*& output) noexcept;

		// =============================================================================================


	};
}


namespace ta
{
	class XmlNode
	{
	public:
		explicit XmlNode(const std::string& name) noexcept;
		~XmlNode(void) noexcept;

		void clear(void) noexcept;


		std::string& getName(void) noexcept;
		const std::string& getName(void) const noexcept;

		void setName(const std::string& name) noexcept;

		const bool hasValue(void) const noexcept;
		std::string& getValue(void) noexcept;
		const std::string& getValue(void) const noexcept;

		void setValue(const std::string& value) noexcept;

		uint32 getChildElementCount(void) const noexcept;

		const XmlNode* getChildElement(const uint32 index) const noexcept;
		XmlNode* getChildElement(const uint32 index) noexcept;

		//const std::unordered_map<std::string, XmlNode*>& getChildElements(void) const noexcept;
		//std::unordered_map<std::string, XmlNode*>& getChildElements(void) noexcept;

		//const XmlNode* getChildElement(const std::string& childName) const noexcept;
		//XmlNode* getChildElement(const std::string& childName) noexcept;

		const std::string* getAttribute(const std::string& attributeName) const noexcept;
		std::string* getAttribute(const std::string& attributeName) noexcept;

		std::unordered_map<std::string, std::string>& getAttributes(void) noexcept;
		const std::unordered_map<std::string, std::string>& getAttributes(void) const noexcept;

		bool addChildElement(XmlNode* childElement) noexcept;
		bool addAttribute(const std::string& name, const std::string& value) noexcept;

	private:
		std::string _name;
		std::string _value;

		//std::unordered_map<std::string, XmlNode*> _childElements;
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
