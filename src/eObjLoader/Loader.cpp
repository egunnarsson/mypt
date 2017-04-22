
/*#include "Defines.h"
#include "Mesh.h"
#include "Log.h"*/


//#include <vector>
//#include <string>
//#include <limits>

/*
namespace {

char stop = '\0';
const unsigned int NO_INDEX = std::numeric_limits<unsigned int>::max();

#define IS_NUM(a) (*(a)>='0'&&*(a)<='9')
#define IS_WS(a)  (*(a)==' '||*(a)=='\t')

void next_line(char *&in)
{
	while (*in && *in != '\n') { ++in; }
	if (*in == '\n') { ++in; }
}

char skip_whitespace(char *&in)
{
	while (*in && IS_WS(in)) { ++in; }
	return *in;
}

bool parse_uint(char *&in, unsigned int &out)
{
	out = 0;
	if (skip_whitespace(in)) {
		if (!IS_NUM(in)) return false;
		while (IS_NUM(in)) {
			out = out * 10 + unsigned int(*in - '0');
			++in;
		}
		return true;
	}
	return false;
}

bool parse_float(char *&in, number &out)
{
	if (skip_whitespace(in)) {
		
		//test cases '-.' '-.0' '0.' whats valid and not... '0.0E+1'

		out = 0;
		number sign;

		if (IS_NUM(in)) {
			sign = 1;
		}
		else {
			sign = *in == '-' ? -1 : (*in == '+' ?  1 : 0);
			if (!sign) {
				return false;
			}
			++in;
		}

		while (IS_NUM(in)) {
			out = out * 10 + number(*in - '0'); 
			++in;
		}

		if (*in == '.') {
			++in;
			number frac = 0.1;
			while (IS_NUM(in)) {
				out += frac * number(*in - '0');
				frac *= 0.1;
				++in;
			}
		}

		if (*in == 'e' || *in == 'E') {
			++in;
			number expSign = 1;
			if (*in == '-') {
				expSign = -1;
				++in;
			}
			else if(*in == '+') {
				++in;
			}
			unsigned int exp;
			if (!parse_uint(in, exp)) {
				return false;
			}
			out = out * pow(10.0, exp * expSign);
		}

		out *= sign;
		
		return true;
	}
	
	return false;
}

template<class v>
bool parse_vector3(char *&in, v &out)
{
	return
		parse_float(in, out.x) &&
		parse_float(in, out.y) &&
		parse_float(in, out.z);
}

bool parse_vector2(char *&in, vec2 &out)
{
	return
		parse_float(in, out.x) &&
		parse_float(in, out.y);
}

std::string parse_word(char *&in)
{
	if (skip_whitespace(in)) {
		char *start = in;

		while (*in && *in!=' ' && *in!='\t' && *in!='\n' && *in!='\r') {
			++in;
		}

		//size_t len = in - start;
		//char *out = (char*)malloc(sizeof(char) * (len+1));
		//memcpy(out, start, len);
		//out[len] = 0;
		return std::string(start, size_t(in - start));
	}
	return std::string();
}

struct index {
	unsigned int position, normal, textcoord;
};

bool parse_index(char *&in, index &out)
{
	if (skip_whitespace(in)) {
		if (!parse_uint(in, out.position)) {
			return false;
		}
		if (*in != '/') {
			--out.position;
			out.normal = out.textcoord = NO_INDEX;
			return true;
			//return false;
		}
		++in;

		if (*in == '/') {
			out.textcoord = 1;
			++in;
		}
		else {
			if (!parse_uint(in, out.textcoord)) {
				return false;
			}
			if (*in != '/') {
				return false;
			}
			++in;
		}

		if (!parse_uint(in, out.normal)) {
			return false;
		}
		// adjust for 0 indexing
		--out.normal; --out.position; --out.textcoord;
		return true;
	}
	return false;
}

char* read_file(const char *fileName, size_t &fileSize)
{
#ifdef _WIN32
	FILE *file = nullptr;
	if (fopen_s(&file, fileName, "rb")) { // ftell can be messed up for non-binary file?
		LOG_WARNING("failed to open file '" + fileName + "'");
		return nullptr;
	}
#else
	FILE *file = fopen(fileName, "rb"); // ftell can be messed up for non-binary file?
#endif
	char *fileContent;
	
	if (file == nullptr) {
		LOG_WARNING("failed to open file '" + fileName + "'");
		return nullptr;
	}
	else {

		fseek(file, 0, SEEK_END);
		fileSize = ftell(file);
		fseek(file, 0, SEEK_SET);

		fileContent = (char*)malloc((fileSize + 1) * sizeof(char));

		if (fileContent != nullptr) {
			// Upon successful completion, fread() shall return the number of elements successfully read
			// which is less than nitems only if a read error or end-of-file is encountered.
			fread(fileContent, sizeof(char), fileSize, file);
			fileContent[fileSize] = 0;

			if (ferror(file)) {
				LOG_ERROR("error reading file '" + fileName + "'");
				free(fileContent);
				fileContent = nullptr;
			}
		}

		fclose(file);

		return fileContent;
	}
}

};*/

/* TODO
Load faces with only one index ie 5 instead of 5/5/5
*/


/*
bool load_obj(const char *fileName, Mesh &mesh)
{
	size_t fileSize;
	char *fileContent = read_file(fileName, fileSize);
	char *pos = fileContent;

	if (fileContent == nullptr) {
		return false;
	}
	
	std::vector<Position> positions;
	std::vector<Direction> normals;
	std::vector<vec2> texCoords;

	std::vector<std::string> mtlLibs;
	std::vector<std::string> chunkNames;
	
	Position p;
	Direction d;
	vec2 v2;

	int currentChunk = -1;
	
	auto makeElement = [&] (index idx1, index idx2, index idx3) -> void {
		/ *Element element;

		element.m_triangle = Triangle(positions[idx1.position], positions[idx2.position], positions[idx3.position]);
		if (idx1.normal == NO_INDEX) {
			element.m_normals[0] = element.m_triangle.m_normal;
			element.m_normals[1] = element.m_triangle.m_normal;
			element.m_normals[2] = element.m_triangle.m_normal;
		}
		else {
			element.m_normals[0] = normals[idx1.normal];
			element.m_normals[1] = normals[idx2.normal];
			element.m_normals[2] = normals[idx3.normal];
		}
		if (idx1.textcoord != NO_INDEX) {
			element.m_textureCoordinates[0] = texCoords[idx1.textcoord];
			element.m_textureCoordinates[1] = texCoords[idx2.textcoord];
			element.m_textureCoordinates[2] = texCoords[idx3.textcoord];
		}

		return element; * /

		mesh.m_chunks[currentChunk].m_triangles.push_back(Triangle(positions[idx1.position], positions[idx2.position], positions[idx3.position]));
		if (idx1.normal == NO_INDEX) {
		}
		else {
			mesh.m_chunks[currentChunk].m_normals.push_back(normals[idx1.normal]);
			mesh.m_chunks[currentChunk].m_normals.push_back(normals[idx2.normal]);
			mesh.m_chunks[currentChunk].m_normals.push_back(normals[idx3.normal]);
		}
		
		if (idx1.textcoord != NO_INDEX) {
			mesh.m_chunks[currentChunk].m_textureCoordinates.push_back(texCoords[idx1.textcoord]);
			mesh.m_chunks[currentChunk].m_textureCoordinates.push_back(texCoords[idx2.textcoord]);
			mesh.m_chunks[currentChunk].m_textureCoordinates.push_back(texCoords[idx3.textcoord]);
		}

	};

	while (*pos) {
		switch (*pos) {
		case '#': // ---------------- comment -----
			break;

		case 'v':
			switch (pos[1]) {
			case ' ': // ---- vertex position -----
			case '\t':
				pos += 2;
				if (parse_vector3(pos, p)) {
					positions.push_back(p);
				}
				else {
					pos = &stop;
				}
				break;

			case 't': // --------- text-coord ----
				pos += 2;
				if (parse_vector2(pos, v2)) {
					texCoords.push_back(v2);
				}
				else {
					pos = &stop;
				}
				break;

			case 'n': // ------ vertex normal ----
				pos += 2;
				if (parse_vector3(pos, d)) {
					normals.push_back(d);
				}
				else {
					pos = &stop;
				}
				break;

			case 'p': // ---------- free form ----
				break;

			default:
				pos = &stop;
				break;
			}
			break;

		case 'f': // ------------------- face ----
			
			++pos;

			index idx1, idx2, idx3, idx4;

			if (!parse_index(pos, idx1)) break;
			if (!parse_index(pos, idx2)) break;
			if (!parse_index(pos, idx3)) break;

			//mesh.m_chunks[currentChunk].push_back(makeElement(idx1, idx2, idx3));
			makeElement(idx1, idx2, idx3);

			if (parse_index(pos, idx4)) { // quad => make two triangles
				//indexList.push_back(indexList[indexList.size()-3]);
				//indexList.push_back(indexList[indexList.size()-2]);
				//indexList.push_back(i);

				//mesh.m_chunks[currentChunk].push_back(makeElement(idx1, idx3, idx4));
				makeElement(idx1, idx3, idx4);
			}

			break;
			
		case 'u': // --------------- material ----
			if (strncmp(pos, "usemtl", 6) == 0) {

				pos += 6;
				// TODO check ' ' || '\t'
				std::string name = parse_word(pos);
				
				if (!name.empty()) {
					// check if same material has already been used...
					currentChunk = -1;
					int chunkCount = int(chunkNames.size());
					for (int i=0; i < chunkCount; ++i) {
						if (name == chunkNames[i]) {
							currentChunk = i;
							break;
						}
					}

					//... otherwise add new chunk
					if (currentChunk == -1) {
						mesh.m_chunks.push_back(Chunk());
						mesh.m_materials.push_back(Material(DIFFUSE, Color(), Color(0.5, 0.5, 0.5)));

						chunkNames.push_back(name);
						currentChunk = int(chunkNames.size() - 1);
					}
				}
				else {
					pos = &stop;
				}
			}
			else {
				pos = &stop;
			}
			break;

		case 'm':
			if (strncmp(pos, "mtllib", 6) == 0) {
				pos += 6;
				std::string mtllib = parse_word(pos);
				if (mtllib.empty()) {
					pos = &stop;
				}
				else {
					mtlLibs.push_back(mtllib);
				}
			}
			break;
		/ * when everything is implemented we can use this
		default:
			pos = &stop;
			break;

		* /
		}

		next_line(pos);
	}

	free(fileContent);

	if (pos == &stop) {
		LOG_WARNING("unable to load model '" + fileName + "'");
		return false;
	}

	return true;
}
*/

#include <algorithm>
#include <functional>
#include <cctype>

#define E_OBJ_LOADER_DLL_EXPORT __declspec(dllexport)
#include "Loader.h"

const unsigned int eObjLoader::NO_INDEX = std::numeric_limits<unsigned int>::max();
const unsigned int eObjLoader::POSITION = 0; 
const unsigned int eObjLoader::NORMAL = 1;
const unsigned int eObjLoader::TEX_COORD = 2;

namespace {

	// http://stackoverflow.com/a/217605/1620667

	// trim from start
	inline std::string &ltrim(std::string &s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
		return s;
	}

	// trim from end
	inline std::string &rtrim(std::string &s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
		return s;
	}

	// trim from both ends
	inline std::string &trim(std::string &s) {
		return ltrim(rtrim(s));
	}

	struct Line {

		std::string m_string;
		std::string::iterator m_it;

		bool readFrom(std::ifstream &stream)
		{
			if (std::getline(stream, m_string).fail())
				return false;
			trim(m_string);
			m_it = m_string.begin();
			return true;
		}

		bool empty() const
		{
			return m_string.empty();
		}

		bool atEnd() const
		{
			return m_it >= m_string.end();
		}

		char get() const
		{
			return m_it < m_string.end() ? *m_it : '\0';
		}

		char get(int i) const
		{
			auto it2 = m_it + i;
			return it2 < m_string.end() ? *it2 : '\0';
		}

		char next()
		{
			++m_it;
			return get();
		}
	};

	bool equal(const std::string &str, const std::string::iterator &other)
	{
		return std::equal(str.begin(), str.end(), other);
	}

	bool skip_whitespace(Line &line)
	{
		line.m_it = std::find_if_not(line.m_it, line.m_string.end(), std::isspace);
		return !line.atEnd();
	}

	bool parse_uint(Line &line, unsigned int &out)
	{
		// TODO cleanup, this is copied from old
		out = 0;
		if (skip_whitespace(line)) {
			if (!std::isdigit(line.get())) return false;
			while (std::isdigit(line.get())) {
				out = out * 10 + unsigned int(line.get() - '0');
				line.next();
			}
			return true;
		}
		return false;
	}

	bool parse_float(Line &line, float &out)
	{
		// TODO cleanup, this is copied from old
		if (skip_whitespace(line)) {

			//test cases '-.' '-.0' '0.' whats valid and not... '0.0E+1'

			out = 0;
			float sign;

			if (std::isdigit(line.get())) {
				sign = 1;
			}
			else {
				sign = line.get() == '-' ? -1.0f : (line.get() == '+' ? 1.0f : 0.0f);
				if (!sign) {
					return false;
				}
				line.next();
			}

			while (std::isdigit(line.get())) {
				out = out * 10 + float(line.get() - '0');
				line.next();
			}

			if (line.get() == '.') {
				line.next();
				float frac = 0.1f;
				while (std::isdigit(line.get())) {
					out += frac * float(line.get() - '0');
					frac *= 0.1f;
					line.next();
				}
			}

			if (line.get() == 'e' || line.get() == 'E') {
				line.next();
				float expSign = 1;
				if (line.get() == '-') {
					expSign = -1;
					line.next();
				}
				else if (line.get() == '+') {
					line.next();
				}
				unsigned int exp;
				if (!parse_uint(line, exp)) {
					return false;
				}
				out = out * powf(10.0f, exp * expSign);
			}

			out *= sign;

			return true;
		}

		return false;
	}

	template<class V>
	bool parse_vector(Line &line, V &out);
	/*{
		assert(0);
		return false;
	}*/

	template<>
	bool parse_vector(Line &line, eObjLoader::vec3 &out)
	{
		return parse_float(line, out.x) && parse_float(line, out.y) && parse_float(line, out.z);
	}

	template<>
	bool parse_vector(Line &line, eObjLoader::Direction &out)
	{
		return parse_float(line, out.x) && parse_float(line, out.y) && parse_float(line, out.z);
	}

	template<>
	bool parse_vector(Line &line, eObjLoader::vec2 &out)
	{
		return parse_float(line, out.x) && parse_float(line, out.y);
	}

	template<class V>
	bool parse(Line &line, std::vector<V> &list)
	{
		V out(0);
		line.m_it += 2;
		if (parse_vector<V>(line, out)) {
			list.push_back(out);
			return true;
		}
		return false;
	}

	bool parse_index(Line &line, eObjLoader::uvec3 &out)
	{
		if (skip_whitespace(line)) {
			if (!parse_uint(line, out[eObjLoader::POSITION])) {
				return false;
			}
			if (line.get() != '/') {
				out = eObjLoader::uvec3(out[eObjLoader::POSITION] - 1, eObjLoader::NO_INDEX, eObjLoader::NO_INDEX);
				return true;
			}

			if (line.next() == '/') {
				out[eObjLoader::TEX_COORD] = 1;
				line.next();
			}
			else {
				if (!parse_uint(line, out[eObjLoader::TEX_COORD])) {
					return false;
				}
				if (line.get() != '/') {
					return false;
				}
				line.next();
			}
			
			if (!parse_uint(line, out[eObjLoader::NORMAL])) {
				return false;
			}
			// adjust for 0 indexing
			--out[eObjLoader::NORMAL]; --out[eObjLoader::POSITION]; --out[eObjLoader::TEX_COORD];
			return true;
		}
		return false;
	}
}

eObjLoader::eObjLoader(const std::string &fileName) :
	m_loaded(false),
	m_stream(fileName),
	m_normalMissing(false),
	m_texCoordMissing(false)
{
}

eObjLoader::~eObjLoader()
{
}

bool eObjLoader::load()
{
	if (m_loaded)
		return true;

	if (!m_stream.is_open()) // print error if file doesn't exists...
		return false;

	assert(m_positions.empty() && m_normals.empty() && m_texCoords.empty() && m_indices.empty());

	// used for error message
	unsigned int lineNum = 0;

	// bleh
	vec2 v2;
	vec3 v3(0);

	bool stop = false;
	Line line;

	for (; !stop && line.readFrom(m_stream); ++lineNum)
	{
		// is there a difference string() string("") string(nullptr)... otherwise case '\0' covers empty
		if (line.empty())
			continue;

		switch (line.get()) {
		case '\0': //------------- empty line -----
		case '#': // ---------------- comment -----
			break;

		case 'v':
			switch (line.get(1)) {
			case ' ': // ---- vertex position -----
			case '\t':
				if (!parse<vec3>(line, m_positions))
					stop = true;
				break;

			case 't': // --------- text-coord ----
				if (!parse<vec2>(line, m_texCoords))
					stop = true;
				break;

			case 'n': // ------ vertex normal ----
				if (!parse<Direction>(line, m_normals))
					stop = true;
				break;

			case 'p': // ---------- free form ----
				break;

			default:
				stop = true;
				break;
			}
			break;

		case 'f': // ------------------- face ----
		{
					  ++line.m_it; // ++pos;

					  uvec3 idx1(NO_INDEX), idx2(NO_INDEX), idx3(NO_INDEX), idx4(NO_INDEX);

					  if (!parse_index(line, idx1)) break;
					  if (!parse_index(line, idx2)) break;
					  if (!parse_index(line, idx3)) break;

					  //makeElement(idx1, idx2, idx3);
					  m_indices.push_back(idx1);
					  m_indices.push_back(idx2);
					  m_indices.push_back(idx3);

					  m_normalMissing |= (idx1[NORMAL] == NO_INDEX || idx2[NORMAL] == NO_INDEX || idx3[NORMAL] == NO_INDEX);
					  m_texCoordMissing |= (idx1[TEX_COORD] == NO_INDEX || idx2[TEX_COORD] == NO_INDEX || idx3[TEX_COORD] == NO_INDEX);

					  if (parse_index(line, idx4)) { // quad => make two triangles
						  //makeElement(idx1, idx3, idx4);
						  m_indices.push_back(idx1);
						  m_indices.push_back(idx3);
						  m_indices.push_back(idx4);

						  m_normalMissing |= (idx1[NORMAL] == NO_INDEX || idx3[NORMAL] == NO_INDEX || idx4[NORMAL] == NO_INDEX);
						  m_texCoordMissing |= (idx1[TEX_COORD] == NO_INDEX || idx3[TEX_COORD] == NO_INDEX || idx4[TEX_COORD] == NO_INDEX);
					  }

					  break;
		}
		case 'u': // --------------- material ----
			if (equal("usemtl", line.m_it)) { //strncmp(pos, "usemtl", 6) == 0) {
				/*
				line.m_it += 6; // pos += 6;
				// TODO check ' ' || '\t'
				std::string name = parse_word(pos);

				if (!name.empty()) {
					// check if same material has already been used...
					currentChunk = -1;
					int chunkCount = int(chunkNames.size());
					for (int i = 0; i < chunkCount; ++i) {
						if (name == chunkNames[i]) {
							currentChunk = i;
							break;
						}
					}

					//... otherwise add new chunk
					if (currentChunk == -1) {
						mesh.m_chunks.push_back(Chunk());
						mesh.m_materials.push_back(Material(DIFFUSE, Color(), Color(0.5, 0.5, 0.5)));

						chunkNames.push_back(name);
						currentChunk = int(chunkNames.size() - 1);
					}
				}
				else {
					stop = true;
				}*/
			}
			else {
				stop = true;
			}
			break;

		case 'm':
			if (equal("mtllib", line.m_it)) { // (strncmp(pos, "mtllib", 6) == 0) {
				/*line.m_it += 6; // pos += 6;
				std::string mtllib = parse_word(pos);
				if (mtllib.empty()) {
					stop = true;
				}
				else {
					mtlLibs.push_back(mtllib);
				}*/
			}
			break;
			/*when everything is implemented we can use this
		default:
			pos = &stop;
			break;

			*/
		}

	}

	return true;
}

const std::vector<eObjLoader::vec3>& eObjLoader::positions()
{
	return m_positions;
}

const std::vector<eObjLoader::Direction>& eObjLoader::normals()
{
	return m_normals;
}

const std::vector<eObjLoader::vec2>& eObjLoader::texCoords()
{
	return m_texCoords;
}

const std::vector<eObjLoader::uvec3>& eObjLoader::indices()
{
	return m_indices;
}

bool eObjLoader::hasMissingNormal() const
{
	return m_normalMissing;
}

bool eObjLoader::hasMissingTexCoord() const
{
	return m_texCoordMissing;
}

void eObjLoader::calculateMissingNormals()
{
	for (auto it = m_indices.begin(); it != m_indices.end(); it += 3)
	{
		uvec3 &index1 = *it;
		uvec3 &index2 = *(it + 1);
		uvec3 &index3 = *(it + 2);

		if (index1[NORMAL] == NO_INDEX || index2[NORMAL] == NO_INDEX || index3[NORMAL] == NO_INDEX)
		{
			Direction normal(1,0,0); // Todo: calculate normal!!!

			m_normals.push_back(normal);
			unsigned int normalIndex = static_cast<unsigned int>(m_normals.size() - 1);

			if (index1[NORMAL] == NO_INDEX)
				index1[NORMAL] = normalIndex;

			if (index2[NORMAL] == NO_INDEX)
				index2[NORMAL] = normalIndex;

			if (index3[NORMAL] == NO_INDEX)
				index3[NORMAL] = normalIndex;

		}
	}
}

void eObjLoader::fillMissingTexCoords(vec2 defaultValue)
{
	m_texCoords.push_back(defaultValue);
	unsigned int defaultIndex = static_cast<unsigned int>(m_texCoords.size() - 1);
	bool added = false;

	for (auto it = m_indices.begin(); it != m_indices.end(); ++it)
	{
		if ((*it)[TEX_COORD] == NO_INDEX)
		{
			(*it)[TEX_COORD] = defaultIndex;
			added = true;
		}
	}

	if (!added)
	{
		m_texCoords.pop_back();
	}
}