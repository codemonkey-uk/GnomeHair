#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

using namespace std;

typedef std::vector< string > Row;

inline bool iscomma( char c ) { return c==','; }
inline bool iseol( char c ) { return c=='\n' || c=='\r'; }
    
template<typename pred >
std::vector< string > Split( const string& in, pred fn )
{
    std::vector< string > result;

    auto last = in.end();
    auto a = in.begin();

    result.reserve( std::count_if( a, last, fn ) );

    while(a!=last) {
        auto b = std::find_if(a, last, fn);
        if (b==last)
            break;
            
        result.push_back( string(a, b) );
        a = ++b;
    }
    
    result.push_back( string(a, last) );

    return result;
}

string Trim( const string& in )
{
    auto i = std::find_if( in.begin(), in.end(), [](int c) {return !std::isspace(c);} );

    // string is empty or contains only space characters, return empty
    if (i==in.end())
        return string();

    auto j = in.end();
    while (isspace(*--j));

    return string(i, ++j);
}

bool isempty(const string& line) { return line.empty(); }

Row ParseLine( const string& line, bool trim=true )
{
    auto row = Split( line, iscomma );
    if (trim)
    {
        for(auto c=row.begin(); c!=row.end(); ++c)
        {
            *c = Trim( *c );
        }
    }
    return row;
}

typedef pair<int,int> Bucket;



struct Colour
{
    string name;
    vector< Bucket > buckets;
    int total = 0;
};

const int bucket_size = 3;
const string tab = "    ";

Bucket GetBucket( int l, int p )
{
    return Bucket( p/bucket_size, l/bucket_size );
}

int main(int argc, char** args)
{
    //cout << argc << endl;
    //while (argc--)
    //    cout << args[argc] << endl;
    
    vector< Colour > colours;
    string line;
    std::ifstream colour_file( "colours.txt" );
    while (getline(colour_file, line))
    {
        colours.push_back({Trim(line)});
    }
    
    cerr << "Loaded " << colours.size() << " colours." << endl;
    
    map< Bucket, int > distributions;
    string header;
    int rows=0;
    int gnomes=0;
    

    
    std::ifstream npc_file( "npc.csv" );
    if (getline(npc_file, header))
    {
        auto h = ParseLine(header);
        
        auto ri = find(h.begin(), h.end(), "race");
        auto rd = distance(h.begin(), ri);
        cerr << "race on column " << rd << endl;

        auto pi = find(h.begin(), h.end(), "pheomelanin");
        auto pd = distance(h.begin(), pi);
        cerr << "pheomelanin on column " << pd << endl;

        auto li = find(h.begin(), h.end(), "lipochrome");
        auto ld = distance(h.begin(), li);
        cerr << "lipochrome on column " << ld << endl;

        while (getline(npc_file, line))
        {
            rows ++;
            auto r = ParseLine(line);
            if (r[rd]=="1")
            {
                gnomes++;
                int l = atoi( r[ld].c_str() );
                int p = atoi( r[pd].c_str() );
                distributions[GetBucket(l,p)]++;
            }
        }
    }
    
    cerr << "Loaded " << rows << " NPCs." << endl;
    cerr << "Found " << gnomes << " gnomes." << endl;
    cerr << "With " << distributions.size() << " unique hormonal signatures." << endl;
    
    int target = ceil((float)gnomes/colours.size());
    cerr << "Target " << target << " gnomes per colour." << endl;
    
    vector< pair< Bucket, int > > buckets( distributions.begin(), distributions.end() );
    sort( buckets.begin(), buckets.end(), [](auto a, auto b){return a.second > b.second;} );
    
    for (const auto& kvp : buckets)
    {
        int best_score=-INT_MAX, best_index=0;
        for (int index=0; index!=colours.size(); ++index)
        {
            // proximity to target now (0 == perfect)
            int a = abs(colours[index].total - target);
            // proximity to target if combined 
            int b = abs(colours[index].total + kvp.second - target);

            // square over-error
            if (colours[index].total + kvp.second > target)
                b=b*b;
            
            // was big now small = good change 
            int score = a-b;
            if (score>best_score)
            {
                best_score = score;
                best_index = index;
            }
            else if (score==best_score)
            {
                // tiebreak equal improvement on current furthest to target
                if (colours[index].total < colours[best_index].total)
                {
                    best_score = score;
                    best_index = index;
                }
            }
        }
        
        colours[best_index].buckets.push_back(kvp.first);
        colours[best_index].total += kvp.second;
    }
    
    vector< string > pad_with;

    cerr << "Results" << endl;
    cerr << "colour, bins, pop" << endl;

    map< Bucket, string > results;
    for (int index=0;index!=colours.size();++index)
    {
        for (auto b : colours[index].buckets)
            results[b] = colours[index].name;
        
        cerr << colours[index].name << ", " 
             << colours[index].buckets.size() << ", " 
             << colours[index].total << endl;

        if (colours[index].total < target)
            pad_with.push_back(colours[index].name);
    }
    int cr=0;
    
    cout << "[" << endl;
    for (int l=0;l<=46;l+=bucket_size)
    {
        if (l>0) cout << "," << endl;
        cout << tab << "[" << endl;
        for (int p=0;p<=46;p+=bucket_size)
        {
            if (p>0) cout << "," << endl;
            
            string cname = results[GetBucket(l,p)];
            if (cname.empty()) cname = pad_with[(cr++)%pad_with.size()];
            
            cout << tab << tab << "[" << '"' << cname << '"' << "]";
        }
        cout << endl << tab << "]";
    }
    cout << endl << "]" << endl;    
    
}
