#include "OBJloader.h"

OBJLoader::OBJLoader()
{

}

OBJLoader::~OBJLoader()
{

}

VertexInputType* OBJLoader::LoadObj(int& vertexCount, int& textureCount, int& normalCount, int& faceCount, string fileName)
{
	ifstream fileIn;
	VertexInputType* vertexInput;

	fileIn.open(fileName, ifstream::in);

	vector<XMFLOAT3> vPos;
	vector<XMFLOAT3> vNor;
	vector<XMFLOAT2> vTC;

	vector<UINT> indPos;
	vector<UINT> indNor;
	vector<UINT> indTex;
	bool loop = true;
	char _input;

	if (fileIn.is_open())
	{
		while (loop)
		{
			_input = fileIn.get();

			switch (_input)
			{

			case '#':
				_input = fileIn.get();
				while (_input != '\n')
					_input = fileIn.get();
				break;
			case 's':
				_input = fileIn.get();
				while (_input != '\n')
					_input = fileIn.get();
				break;
			case 'u':
				_input = fileIn.get();
				while (_input != '\n')
					_input = fileIn.get();
				break;
			case 'm':
				_input = fileIn.get();
				while (_input != '\n')
					_input = fileIn.get();
				break;
			case 'g':
				_input = fileIn.get();
				while (_input != '\n')
					_input = fileIn.get();
				break;
			case 'v':
				_input = fileIn.get();
				if (_input == ' ')
				{
					float fx, fy, fz;
					fileIn >> fx >> fy >> fz;
					vPos.push_back(XMFLOAT3(fx, fy, fz));
					vertexCount++;
				}
				if (_input == 't')
				{
					float txx, txz;
					fileIn >> txx >> txz;
					vTC.push_back(XMFLOAT2(txx, 1 - txz));
					textureCount++;
				}
				if (_input == 'n')
				{
					float nx, ny, nz;
					fileIn >> nx >> ny >> nz;
					vNor.push_back(XMFLOAT3(nx, ny, nz));
					normalCount++;

				}
				break;
			case 'f':
				_input = fileIn.get();

				if (_input == ' ')
				{

					while (_input != '\n')
					{

						int indP, indT, indN;

						char pop;

						fileIn >> indP >> pop >> indT >> pop >> indN;

						indNor.push_back(indN);

						indTex.push_back(indT);

						indPos.push_back(indP);

						faceCount++;

						_input = fileIn.get();

					}
				}
				break;
			}

			if (fileIn.fail())
			{
				loop = false;
			}
		}

		vertexInput = new VertexInputType[faceCount];
		for (int i = 0; i < faceCount; i++)
		{
			vertexInput[i].position = vPos[indPos[i] - 1];
			vertexInput[i].uv = vTC[indTex[i] - 1];
			vertexInput[i].normal = vNor[indNor[i] - 1];
		}
		//Clearing Memory
		vector<XMFLOAT3>().swap(vPos);
		vector<XMFLOAT3>().swap(vNor);
		vector<XMFLOAT2>().swap(vTC);
		//Clearing Memory
		vector<UINT>().swap(indPos);
		vector<UINT>().swap(indNor);
		vector<UINT>().swap(indTex);

		fileIn.close();
		return vertexInput;
	}

	return false;
}

bool OBJLoader::ReadColourCounts(int& kdCount, int& kaCount, int& tfCount, int& niCount, string fileName)
{
	ifstream _file;
	char _input;

	// Initialize the counts.
	kdCount = 0;
	kaCount = 0;
	tfCount = 0;
	niCount = 0;

	// Open the file.
	_file.open(fileName);

	// Check if it was successful in opening the file.
	if (_file.fail() == true)
	{
		return false;
	}

	// Read from the file and continue to read until the end of the file is reached.
	_file.get(_input);
	while (!_file.eof())
	{
		// If the line starts with 'K' then count either the kd or the ka.
		if (_input == 'K')
		{
			_file.get(_input);
			if (_input == 'd') 
			{ 
				kdCount++; 
			}
			else if (_input == 'a') 
			{ 
				kaCount++; 
			}
		}	
		else if (_input == 'T') // If the line starts with 'T' then increment the tf count.
		{
			_file.get(_input);
			if (_input == 'f') 
			{ 
				tfCount++;
			}
		}
		else if (_input == 'N') // If the line starts with 'N' then increment the ni count.
		{
			_file.get(_input);
			if (_input == 'i')
			{
				niCount++;
			}
		}

		// Otherwise read in the remainder of the line.
		while (_input != '\n')
		{
			_file.get(_input);
		}

		// Start reading the beginning of the next line.
		_file.get(_input);
	}

	// Close the file.
	_file.close();

	return true;
}

//loading color and tex
ID3D11ShaderResourceView* OBJLoader::LoadColour(string fileName, XMFLOAT3 *RGBDeffuse, XMFLOAT3 *RGBAL, XMFLOAT3 *Tf, XMFLOAT3 *Ni, wstring& texFileName)
{
	ifstream _fin;
	char _input;
	int _kdIndex, _kaIndex, _tfIndex, _niIndex;

	// Starts the index at 0
	_kdIndex = 0;
	_kaIndex = 0;
	_tfIndex = 0;
	_niIndex = 0;
	
	_fin.open(fileName);

	// Check if it was successful in opening the file.
	if (_fin.fail() == true)
	{
		return false;
	}

	_fin.get(_input);

	while (!_fin.eof())
	{
		if (_input == 'K')
		{
			_fin.get(_input);

			if (_input == 'd')
			{
				_fin >> RGBDeffuse[_kdIndex].x >> RGBDeffuse[_kdIndex].y >> RGBDeffuse[_kdIndex].z;
				_kdIndex++;
			}

			if (_input == 'a')
			{
				_fin >> RGBAL[_kaIndex].x >> RGBAL[_kaIndex].y >> RGBAL[_kaIndex].z;
				_kaIndex++;
			}
		}
		else if (_input == 'T')
		{
			_fin.get(_input);
			if (_input == 'f')
			{
				_fin >> Tf[_tfIndex].x >> Tf[_tfIndex].y >> Tf[_tfIndex].z;
				_tfIndex++;
			}
		}
		else if (_input == 'N')
		{
			_fin.get(_input);
			if (_input == 'i')
			{
				_fin >> Ni[_niIndex].x;
				_niIndex++;
			}
		}
		else if (_input == 'm')
		{
			_fin.get(_input);
			while (_input != '\n' && _input != '_')
			{
				_fin.get(_input);
			}
			_fin.get(_input);
			if (_input == 'K')
			{
				_fin.get(_input);
				if (_input == 'd')
				{
					_fin.get(_input);
					_fin.get(_input);
					while (_input != '\n')
					{
						texFileName += _input;
						_fin.get(_input);
					}
				}
			}
			else if (_input == 'G')
			{
				_fin.get(_input);
				if (_input == 'l')
				{
					_fin.get(_input);
					_fin.get(_input);
					while (_input != '\n')
					{
						_fin.get(_input);
					}
				}
			}		
		}

		while (_input != '\n')
		{
			_fin.get(_input);
		}

		_fin.get(_input);
	}
	return nullptr;
}
