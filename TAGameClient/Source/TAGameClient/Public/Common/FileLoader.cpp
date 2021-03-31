#include "Common/FileLoader.h"
#include "Common/StringUtility.h"
#include "Common/Vector.h"
#include "Common/Serializer.h"

#include <fstream>


namespace ta
{
	bool FileLoader::getFilePathsFromDirectory(const fs::path& directoryPath, std::vector<fs::path>& output) noexcept
	{
		if (false == fs::exists(directoryPath))
		{
			
			TA_ASSERT_DEV(false, "해당 경로에 없습니다. %s", directoryPath.c_str());
			return false;
		}

		output.clear();
		for (const fs::directory_entry& entry : fs::directory_iterator(directoryPath))
		{
			output.push_back(entry.path());
		}

		return true;
	}

	bool FileLoader::loadXml(const fs::path filePath, XmlNode* rootOutput) noexcept
	{
		if (nullptr == rootOutput)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		MemoryBuffer tempBuffer;
		if (false == FileLoader::loadFileString(filePath, tempBuffer))
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		// xml파일은 중간에 null문자가 없다 그냥 가지고 와도 된다.
		std::string fileString(tempBuffer.getData());
		std::vector<std::string> splitedStrings;
		Split(fileString, "<>", splitedStrings);
		const uint32 splitedStringCount = splitedStrings.size();

		uint32 index = 0;
		std::vector<XmlNode*> elementStack;

		// 루트 노드부터 찾는다.
		uint32 stringCount = splitedStrings.size();
		while (index < stringCount)
		{
			if (0 == splitedStrings[index].compare("Root"))
			{
				rootOutput->setName("Root");
				elementStack.push_back(rootOutput);
				index += 2;
				break;
			}
			
			++index;
		}

		if (stringCount == index)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}
		// 일단 루트 바깥으로 나왔다. <Root> 이후

		if (index == stringCount)
		{
			TA_ASSERT_DEV(false, "루트노드가 없습니다.");
			return false;
		}

		while (false == elementStack.empty())
		{
			if (0 == splitedStrings[index].compare("<"))
			{
				++index;
				// element end case // ex) </ElementName>
				if ('/' == splitedStrings[index].front())
				{
					// 자식노드가없고 <Name> value </Name> 형식으로 끝났을때
					if (0 == elementStack.back()->getChildElementCount())
					{
						elementStack.back()->setValue(splitedStrings[index - 2]);
					}
					elementStack.pop_back();
				}
				else
				{
					XmlNode* childElement = nullptr;
					// GroupGameData Key="1" MoveGameDataKey="1" ...
					if (false == FileLoader::loadXmlNodeFromString(splitedStrings[index], childElement))
					{
						TA_ASSERT_DEV(false, "비정상입니다.");
						return false;
					}

					elementStack.back()->addChildElement(childElement);
					elementStack.push_back(childElement);
				}
			}
			else if (0 == splitedStrings[index].compare(">"))
			{
				// element end case // ex) <ElementName Attribute1="Value1"/> 
				if ('/' == splitedStrings[index - 1].back())
				{
					elementStack.pop_back();
				}
			}

			++index;
		}
		
		return true;
	}

	bool FileLoader::saveXml(const fs::path filePath, const XmlNode* rootInput) noexcept
	{
		if (nullptr == rootInput)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		if (0 != rootInput->getName().compare("Root"))
		{
			TA_ASSERT_DEV(false, "root xml node's name is must be 'Root'");
			return false;
		}

		std::string xmlString("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>");
		
		std::vector<const XmlNode*> elementStack;
		elementStack.push_back(rootInput);

		if (false == FileLoader::makeElementString(elementStack, xmlString))
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		MemoryBuffer tempBuffer;
		tempBuffer.write(xmlString.c_str(), xmlString.size(), TADataType::StdString);
		if (false == FileLoader::saveFileString(filePath, tempBuffer))
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		return true;
	}

	bool FileLoader::loadObj(const fs::path filePath, ObjGroup* objGroupOutput) noexcept
	{
		if (nullptr == objGroupOutput)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		MemoryBuffer tempBuffer;
		if (false == FileLoader::loadFileString(filePath, tempBuffer))
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		std::string tempFileString(tempBuffer.getData());
		std::string fileString;
		Trim(tempFileString, fileString, "\t");

		std::vector<std::string> splitedStrings;
		Split(fileString, "\n ", splitedStrings);
		const uint32 splitedStringCount = splitedStrings.size();

		uint32 startLinePos = 0;
		uint32 endLinePos = 0;

		uint32 stringCount = splitedStrings.size();
		for (uint32 index = 0; index < stringCount; ++index)
		{
			//objGroupOutput->addObjLineChunck()
			if (splitedStrings[index].compare("\n"))
			{
				std::string keyName = splitedStrings[startLinePos];
				ObjLineChunck* newChunk = new ObjLineChunck;
				for (uint32 linePos = startLinePos + 1; linePos < index; ++linePos)
				{
					if (0 == splitedStrings[index].compare(" "))
					{
						continue;
					}

					newChunk->_lineDataSet.push_back(splitedStrings[index]);
				}

				objGroupOutput->addObjLineChunck(keyName, newChunk);
			}
		}
		
		return true;
	}

	bool FileLoader::saveFileString(const fs::path& filePath, const MemoryBuffer& buffer) noexcept
	{
		std::ofstream out(filePath);
		if (false == out.is_open())
		{
			TA_ASSERT_DEV(false, "비정상입니다. %s", filePath.c_str());
			return false;
		}

		out.write(buffer.getData(), buffer.getDataSize());
		//out.flush();
		out.close();

		return true;
	}

	bool FileLoader::saveFileString(const fs::path& filePath, const std::string& buffer) noexcept
	{
		std::ofstream out(filePath);
		if (false == out.is_open())
		{
			TA_ASSERT_DEV(false, "비정상입니다. %s", filePath.c_str());
			return false;
		}

		out.write(buffer.c_str(), buffer.size());
		//out.flush();
		out.close();

		return true;
	}

	bool FileLoader::loadFileString(const fs::path& filePath, MemoryBuffer& buffer) noexcept
	{
		std::ifstream in;
		in.open(filePath);
		if (false == in.is_open())
		{
			TA_ASSERT_DEV(false, "비정상입니다. %s", filePath.c_str());
			return false;
		}

		in.seekg(0, std::ios::end); // end에서 0떨어진곳으로 이동
		int64 stringSize = static_cast<int64>(in.tellg());
		in.seekg(0, std::ios::beg); // begin에서 0떨어진곳으로 이동

		buffer.prepareCopyBuffer(stringSize);
		in.read(buffer.getData(), stringSize);

		in.close();
		return true;
	}


	bool FileLoader::loadBinaryFile(const fs::path& filePath, MemoryBuffer& buffer) noexcept
	{
		std::ifstream in(filePath, std::ifstream::binary);
		if (false == in.is_open())
		{
			TA_ASSERT_DEV(false, "비정상입니다. %s", filePath.c_str());
			return false;
		}

		in.seekg(0, std::ios::end); // end에서 0떨어진곳으로 이동
		int64 stringSize = static_cast<int64>(in.tellg());
		in.seekg(0, std::ios::beg); // begin에서 0떨어진곳으로 이동

		buffer.prepareCopyBuffer(stringSize);
		in.read(buffer.getData(), stringSize);

		in.close();
		return true;
	}

	bool FileLoader::saveBinaryFile(const fs::path& filePath, const MemoryBuffer& buffer) noexcept
	{
		std::ofstream out(filePath, std::ios::out | std::ios::binary);
		if (false == out.is_open())
		{
			TA_ASSERT_DEV(false, "비정상입니다. %s", filePath.c_str());
			return false;
		}

		out.write(buffer.getData(), buffer.getDataSize());
		//out.flush();
		out.close();

		return true;
	}

	bool FileLoader::makeElementString(std::vector<const XmlNode*>& elementStack, std::string& output) noexcept
	{
		const uint32 elementStackSize = elementStack.size();
		if (0 == elementStackSize)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		const XmlNode* currentElement = elementStack.back();
		const uint32 childElementCount = currentElement->getChildElementCount();
		const bool hasChildElement = (0 != childElementCount);
		const bool hasValue = (true == currentElement->hasValue());
		if (true == hasChildElement && true == hasValue)
		{
			TA_ASSERT_DEV(false, "child가 있으면서 value가 있을 수 없습니다.");
			return false;
		}

		std::string depthString;
		FileLoader::makeDepthString(elementStackSize, depthString);
		output.append(depthString);

		FileLoader::makeAttributeString(currentElement, output);

		const XmlNode* childElement = nullptr;
		//const std::unordered_map<std::string, XmlNode*>&  childElements = currentElement->getChildElements();
		//std::unordered_map<std::string, XmlNode*>::const_iterator it = childElements.begin();
		//const std::unordered_map<std::string, XmlNode*>::const_iterator end = childElements.end();

		for(uint32 index =0; index < childElementCount; ++index)
		{
			childElement = currentElement->getChildElement(index);
			elementStack.push_back(childElement);
			if (false == FileLoader::makeElementString(elementStack, output))
			{
				TA_ASSERT_DEV(false, "비정상입니다.");
				return false;
			}
		}

		// </Name>
		if (true == hasChildElement) // 값이 없고 자식 엘리먼트가 있는 경우 
		{
			output.append(depthString + "</" + currentElement->getName() + ">");
		}
		else if(true == hasValue) // 값이 있는 리프노드
		{
			output.append(currentElement->getValue() + "</" + currentElement->getName() + ">");
		}

		elementStack.pop_back();

		return true;
	}

	void FileLoader::makeDepthString(const uint32 depth, std::string& output) noexcept
	{
		output = "\n";
		for (uint32 depthIndex = 1; depthIndex < depth; ++depthIndex)
		{
			output.append("\t");
		}
	}

	void FileLoader::makeAttributeString(const XmlNode* input, std::string& output) noexcept
	{
		// <Name Att1="Value1" Att1="Value2" Att1="Value3"> or <Name Att1="Value1" Att1="Value2" Att1="Value3"/>
		TA_ASSERT_DEV((nullptr != input), "비정상");
		output.append("<" + input->getName());
		const bool hasChildElement = (0 != input->getChildElementCount());
		const bool hasValue = (true == input->hasValue());
		const std::unordered_map<std::string, std::string>& attributes = input->getAttributes();

		std::unordered_map<std::string, std::string>::const_iterator it = attributes.begin();
		const std::unordered_map<std::string, std::string>::const_iterator end = attributes.end();
		for (; it != end; ++it)
		{
			output.append(" ");
			output.append(it->first);
			output.append("=\"");
			output.append(it->second);
			output.append("\"");
		}

		((true == hasChildElement || true == hasValue) ? output.append(">") : output.append("/>"));
	}

	bool FileLoader::loadXmlNodeFromString(const std::string& nodeString, XmlNode*& output) noexcept
	{
		if (nullptr != output)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		// GroupGameData Key="1" MoveGameDataKey="1" ...
		std::vector<std::string> attributeStrings;
		Split(nodeString, "=\" ", attributeStrings);

		const uint32 count = attributeStrings.size();
		if (0 == count)
		{
			TA_ASSERT_DEV(false, "엘리먼트에 데이터가 없습니다.");
			return false;
		}

		output = new XmlNode(attributeStrings[0]);

		std::string keyValueString;
		std::string finalValueString;
		for (uint32 index = 0; index < count; ++index)
		{
			if (0 == attributeStrings[index].compare("="))
			{
				// name : attributeStrings[index-1]
				// value : attributeStrings[index+2] //  attributeStrings[index+2]가 \"이라면 비어있는 문자열을 넣는다.
				// \"가 나올때까지 집어넣는다. 공백있는 값도 받기 위함
				keyValueString.clear();
				finalValueString.clear();
				keyValueString = attributeStrings[index - 1];

				index += 2; // 만약에 "" 이런식으로 되어있으면 finalValueString에 빈문자열이 들어간다.
				while (0 != attributeStrings[index].compare("\""))
				{
					finalValueString += attributeStrings[index];
					++index;
				}

				const bool rv = output->addAttribute(keyValueString, finalValueString);
				if (false == rv)
				{
					TA_ASSERT_DEV(false, "중복된 값이 들어갔습니다.");
					return false;
				}
			}
		}

		return true;
	}
}


namespace ta
{
	XmlNode::XmlNode(const std::string& name) noexcept
		: _name(name)
	{
	}

	XmlNode::~XmlNode(void) noexcept
	{
		clear();
	}

	void XmlNode::clear(void) noexcept
	{
		//std::unordered_map<std::string, XmlNode*>::iterator it = _childElements.begin();
		//const std::unordered_map<std::string, XmlNode*>::const_iterator end = _childElements.end();
		//while (end != it)
		//{
		//	delete it->second;
		//	it->second = nullptr;
		//}
		//
		//_childElements.clear();

		const uint32 count = _childElements.size();
		for (uint32 index = 0; index < count; ++index)
		{
			delete _childElements[index];
			_childElements[index] = nullptr;
		}

		_childElements.clear();
	}

	std::string& XmlNode::getName(void) noexcept
	{
		return _name;
	}
	
	const std::string& XmlNode::getName(void) const noexcept
	{
		return _name;
	}

	void XmlNode::setName(const std::string& name) noexcept
	{
		_name = name;
	}

	const bool XmlNode::hasValue(void) const noexcept
	{
		return (false == _value.empty());
	}

	std::string& XmlNode::getValue(void) noexcept
	{
		return _value;
	}

	const std::string& XmlNode::getValue(void) const noexcept
	{
		return _value;
	}

	void XmlNode::setValue(const std::string& value) noexcept
	{
		_value = value;
	}

	uint32 XmlNode::getChildElementCount(void) const noexcept
	{
		return _childElements.size();
	}

	const XmlNode* XmlNode::getChildElement(const uint32 index) const noexcept
	{
		return _childElements[index];
	}
	
	XmlNode* XmlNode::getChildElement(const uint32 index) noexcept
	{
		return _childElements[index];
	}
	
	//const std::unordered_map<std::string, XmlNode*>& XmlNode::getChildElements(void) const noexcept
	//{
	//	return _childElements;
	//}
	//
	//std::unordered_map<std::string, XmlNode*>& XmlNode::getChildElements(void) noexcept
	//{
	//	return _childElements;
	//}
	//
	//const XmlNode* XmlNode::getChildElement(const std::string& childName) const noexcept
	//{
	//	const std::unordered_map<std::string, XmlNode*>::const_iterator rv = _childElements.find(childName);
	//
	//	if (_childElements.end() == rv)
	//	{
	//		return nullptr;
	//	}
	//
	//	return (rv->second);
	//}
	//
	//XmlNode* XmlNode::getChildElement(const std::string& childName) noexcept
	//{
	//	const std::unordered_map<std::string, XmlNode*>::iterator rv = _childElements.find(childName);
	//
	//	if (_childElements.end() == rv)
	//	{
	//		return nullptr;
	//	}
	//
	//	return (rv->second);
	//}

	const std::string* XmlNode::getAttribute(const std::string& attributeName) const noexcept
	{
		std::unordered_map<std::string, std::string>::const_iterator rv = _attributes.find(attributeName);

		if (_attributes.end() == rv)
		{
			return nullptr;
		}

		return &(rv->second);
	}

	std::string* XmlNode::getAttribute(const std::string& attributeName) noexcept
	{
		std::unordered_map<std::string, std::string>::iterator rv = _attributes.find(attributeName);

		if (_attributes.end() == rv)
		{
			return nullptr;
		}

		return &(rv->second);
	}

	std::unordered_map<std::string, std::string>& XmlNode::getAttributes(void) noexcept
	{
		return _attributes;
	}

	const std::unordered_map<std::string, std::string>& XmlNode::getAttributes(void) const noexcept
	{
		return _attributes;
	}

	bool XmlNode::addChildElement(XmlNode* childElement) noexcept
	{
		//std::pair<std::unordered_map<std::string, XmlNode*>::iterator, bool> rv
		//	= _childElements.insert(std::pair<std::string, XmlNode*>(childElement->getName(), childElement));
		//
		//if (false == rv.second)
		//{
		//	TA_ASSERT_DEV(false, "같은 Element가 삽입되었습니다. name : %s", ToTstring(childElement->getName()).c_str());
		//	return false;
		//}

		if (nullptr == childElement)
		{
			TA_ASSERT_DEV(false, "비정상");
			return false;
		}

		_childElements.push_back(childElement);

		return true;
	}

	bool XmlNode::addAttribute(const std::string& name, const std::string& value) noexcept
	{
		std::pair<std::unordered_map<std::string, std::string>::iterator, bool> rv
			= _attributes.insert(std::pair<std::string, std::string>(name, value));

		if (false == rv.second)
		{
			TA_ASSERT_DEV(false, "같은 Attribute가 삽입되었습니다. name : %s, value : %s", ToTstring(name).c_str(), ToTstring(value).c_str());
			return false;
		}

		return true;
	}
}


namespace ta
{
	ObjLineChunck::ObjLineChunck(void) noexcept
	{
	}

	ObjLineChunck::~ObjLineChunck(void) noexcept
	{
	}
}


namespace ta
{
	ObjGroup::ObjGroup(void) noexcept
	{
	}
	
	ObjGroup::~ObjGroup(void) noexcept
	{
		clear();
	}

	void ObjGroup::clear(void) noexcept
	{
		std::unordered_multimap<std::string, ObjLineChunck*>::iterator it = _objGroupMap.begin();
		while (_objGroupMap.end() != it)
		{
			if (nullptr != it->second)
			{
				delete it->second;
			}

			it = _objGroupMap.erase(it);
		}
	}

	std::pair<ObjGroup::ObjGroupMap::const_iterator, ObjGroup::ObjGroupMap::const_iterator> ObjGroup::getObjLineChuncks(const std::string& keyName) const noexcept
	{
		return _objGroupMap.equal_range(keyName);
	}

	void ObjGroup::addObjLineChunck(const std::string& keyName, const std::vector<std::string>& chunkStrings) noexcept
	{
		ObjLineChunck* newLineChunk = new ObjLineChunck;
		newLineChunk->_lineDataSet = chunkStrings;
		_objGroupMap.insert(std::pair(keyName, newLineChunk));
	}

	void ObjGroup::addObjLineChunck(const std::string& keyName, const std::string& lineChunkString) noexcept
	{
		ObjLineChunck* newLineChunk = new ObjLineChunck;

		std::vector<std::string> splitedStrings;
		Split(lineChunkString, " ", splitedStrings);
		const uint32 splitedStringCount = splitedStrings.size();

		for (uint32 index = 0; index < splitedStringCount; ++index)
		{
			// "value1 value2 value3" 
			if (0 == splitedStrings[index].compare(" "))
			{
				continue;
			}

			newLineChunk->_lineDataSet.push_back(splitedStrings[index]);
		}

		_objGroupMap.insert(std::pair(keyName, newLineChunk));
	}

	void ObjGroup::addObjLineChunck(const std::string& keyName, ObjLineChunck* chunk) noexcept
	{
		_objGroupMap.insert(std::pair(keyName, chunk));
	}
}