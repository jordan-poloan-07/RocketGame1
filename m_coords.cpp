#include "headers.hpp"

void populateCoordinates(const char* filename, int permBatchIncrement, vector<vector<int> >& meteorCoords)
{
	const int screenX = 640; // given as globals
	const int screenY = 480;

	ifstream file(filename);

	vector<int> xAddends;
	vector<float> yMultipliers;
	vector< vector<int> > meteorCoordinates;

	unsigned int meteorBatch;
	unsigned int meteorsPerBatch;
	unsigned int batchIncrement;

	const int permInc = permBatchIncrement;

	if(file.is_open())
	{
		string s;

		file >> meteorBatch ;
		file >> meteorsPerBatch ;
		file >> batchIncrement;
		file.get();

		while(!file.eof())
		{
			s.erase(s.begin(),s.end());
			getline(file, s);
			
			if( s == string("divider") )
			{
				break;
			}
			else
			{
				int xTempo;
				stringstream conv(s);
				conv >> xTempo;

				xAddends.push_back(xTempo);
			}

		}

		while(!file.eof())
		{
			s.erase(s.begin(),s.end());
			getline(file, s);
			
			if( s == string("divider") )
			{
				break;
			}
			else
			{
				float yFTempo;
				stringstream conv(s);
				conv >> yFTempo;

				yMultipliers.push_back(yFTempo);
			}

		}
	}

	srand(time(NULL));

	for(size_t i = 0; i < meteorBatch; i++)
    {
        for(size_t j = 0; j < meteorsPerBatch; j++)
        {
            vector<int> x_y_coords;

            // for x
            x_y_coords.push_back( screenX + batchIncrement + ( xAddends[rand() % xAddends.size()] ));

            // for y
            x_y_coords.push_back( screenY * ( yMultipliers[rand() % yMultipliers.size()] ) );

            // put it to coords
            meteorCoords.push_back(x_y_coords);
        }

		batchIncrement += permInc;
    }
}