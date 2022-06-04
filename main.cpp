#include <bits/stdc++.h>
using namespace std;

fstream schemafile;

void fetchSchema(string tableName, vector<string> &schema);
void tokenize(string s, string del, vector<string> &dtypes)
{
    int start = 0;
    int end = s.find(del);
    while (end != -1)
    {
        dtypes.push_back(s.substr(start, end - start));

        start = end + del.size();
        end = s.find(del, start);
    }

    dtypes.push_back(s.substr(start, end - start));
}

//to check if table exists in database
bool isTable(string table_name)
{
    schemafile.open("Schema.txt", ios::in);
    string line;
    bool flag = false;

    while(getline(schemafile, line))
    {
        if(line.substr(0, line.find('#')) == table_name)
        {
            flag = true;
            break;
        }
    }

    schemafile.close();
    return flag;
}

//table creation

void createTable(vector<string> cmd){
     string table_name = cmd[2];
    if(isTable(table_name))
    {
        cout << "Table Already Exists!" << endl;
        return;
    }

    schemafile.open("Schema.txt", ios::app);


    schemafile << table_name;

    int start = -1, end = -1;
    for (int i = 3; i < cmd.size(); i++)
    {
        if (cmd[i] == "(")
        {
            start = i;
        }
        if (cmd[i] == ")")
        {
            end = i;
        }
    }

    if (start == -1 || end == -1)
    {
        cout << "Syntax Error (Round Bracket Missing)!! Please Recheck" << endl;
        return;
    }

    while (start < end - 1)
    {
        start++;
        if (cmd[start] != ",")
        {
            schemafile << "#" << cmd[start];
        }
    }
    schemafile << endl;
    cout << "Table created successfully" << endl;

    schemafile.close();
}

void dropTable(vector<string> cmd){
     string table_name = cmd[2];
    if(!isTable(table_name))
    {
        cout<<"Table" << table_name << "doesn't exist"<<endl;
        return;
    }
    schemafile.open("Schema.txt", ios::in);

    fstream temp;
    temp.open("temp.txt", ios::out);


    string line;

    while (getline(schemafile, line))
    {
        string l1 = line.substr(0, line.find('#'));
        if (table_name != l1)
        {
            temp << line << endl;
        }
    }

    schemafile.close();
    temp.close();

    remove("Schema.txt");
    rename("temp.txt", "Schema.txt");

    cout << "Table dropped successfully" << endl;
}

void describe(vector<string> cmd){
    schemafile.open("Schema.txt", ios::in);

    if (cmd.size() > 1)
    {
        string table_name = cmd[1];
        string line;

        while (getline(schemafile, line))
        {
            string l1 = line.substr(0, line.find('#'));

            if (table_name == l1)
            {
                string l2 = line.substr(line.find('#') + 1, line.size());
                int cnt = 1;

                for (auto i : l2)
                {
                    if (i == '#')
                    {
                        if (cnt % 2 != 0)
                        {
                            cout << "--";
                        }
                        else
                        {
                            cout << endl;
                        }
                        cnt++;
                    }
                    else
                    {
                        cout << i;
                    }
                }
            }
        }
    }
    else
    {
        cout << "please mention table name" << endl;
    }
}

void getDatatype(string tableName, vector<string>& data)
{
    vector<string> schema;
    fetchSchema(tableName,schema);
    for(int i = 2; i < schema.size(); i+=2)
    {
        data.push_back(schema[i]);
    }
}

bool isNumber(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it))
        ++it;
    return !s.empty() && it == s.end();
}

//to insert values into table
void insert(vector<string> query)
{
    string table_name = query[2];
    if(!isTable(table_name))
    {
        cout<<"Table does not Exist"<<endl;
        return;
    }

    fstream table;
    table.open(table_name + ".txt", ios::app);

    vector<string> data;
    getDatatype(table_name, data);
    vector<string>datatoinsert;
    for( int i = 5 ; i < query.size()-2 ; i++)
    {

        if(query[i]!= ",")
        {
            datatoinsert.push_back(query[i]);

        }
    }
    if(datatoinsert.size()!= data.size())
    {

        cout<<" please check number of columns";
        return ;
    }



    int start = -1, end = -1;
    for (int i = 4; i < query.size(); i++)
    {
        if (query[i] == "(")
        {
            start = i;
        }

        if (query[i] == ")")
        {
            end = i;
        }
    }

    if (start == -1 || end == -1)
    {
        cout << "Syntax Error (Round Bracket Missing)!! Please Recheck" << endl;
        return;
    }

    int k=0;
    bool invalidType=false;
    int st=start;

    while (st < end - 1)
    {
        st++;
        if (query[st] != ",")
        {
            if( ( data[k]=="int" && !isNumber(query[st]) ) || ( data[k]!="int" && isNumber(query[st]) ))
            {
                cout << "Data Type of " << query[st] << " is invalid." << endl;
                return;
            }
            k++;
        }
    }

    while (start < end - 1)
    {
        start++;
        if (query[start] != ",")
        {
            table << query[start];
            if (start != end - 1)
            {
                table << "#";
            }
            else
            {
                table << "\n";
            }
        }
    }

    cout << "Record added successfully!" << endl;
}
void truncate(vector<string> &querytocons)
{
    string schema = "schema.txt";
    string tname = querytocons[2];
    string deleteline;
    if (querytocons.size() < 2)
    {
        cout << "Incorrect Query" << endl;
        return;
    }

    if (!isTable(tname))
    {
        cout << "Table does not exists.." << endl;
        return;
    }
    else
    {
        fstream table;
        ofstream fout;

        string toRemove = tname + ".txt";
        char char_array[toRemove.size() + 1];
        strcpy(char_array, toRemove.c_str());

        int r = remove(char_array);

        std::ofstream ofs;
        ofs.open(toRemove, std::ofstream::out | std::ofstream::trunc);
        ofs.close();
    }
}

void delete_(vector<string> cmd){

    vector<string> schema;
    fetchSchema(cmd[2], schema);
    int count = 0;

    if (!schema.empty())
    {
        if (cmd.size() == 3)
        {
            string table = cmd[2] + ".txt";
            char c[table.size() + 1];
            strcpy(c, table.c_str());
            remove(c);
        }
        else if (cmd[3] == "where")
        {
            string table_name = cmd[2];
            fstream table;
            table.open(table_name + ".txt", ios::in);
            string line;

            int i, j;
            j = 0;
            int flag = 0;
            vector<string> lineVec;

            fstream temp;
            temp.open("temp.txt", ios::out);

            while (getline(table, line))
            {
                stringstream ss(line);
                while (ss.good())
                {
                    string substr;
                    getline(ss, substr, '#');
                    lineVec.push_back(substr);
                }

                for (i = 1; i < schema.size(); i += 2)
                {
                    if (cmd[4] == schema[i])
                    {
                        if (cmd[5] == "=")
                        {
                            if (cmd[6] == lineVec[j])
                            {
                                flag = 1;
                                count++;
                            }
                        }
                        if (cmd[5] == ">")
                        {
                            if (lineVec[j] < cmd[6])
                            {
                                flag = 1;
                                count++;
                            }
                        }
                        if (cmd[5] == "<")
                        {
                            if (lineVec[j] < cmd[6] )
                            {
                                flag = 1;
                                count++;
                            }
                        }
                        if (cmd[5] == ">=")
                        {
                            if ( lineVec[j] >= cmd[6])
                            {
                                flag = 1;
                                count++;
                            }
                        }
                        if (cmd[5] == "<=")
                        {
                            if (lineVec[j] <= cmd[6])
                            {
                                flag = 1;
                                count++;
                            }
                        }
                        if (cmd[5] == "!=")
                        {
                            if (cmd[6] != lineVec[j])
                            {
                                flag = 1;
                                count++;
                            }
                        }
                    }
                    j++;
                }
                if (flag != 1)
                {
                    temp << line << endl;
                }
                flag = 0;
            }
            table.close();
            temp.close();
            string table1 = table_name + ".txt";
            char c[table1.size() + 1];
            strcpy(c, table1.c_str());
            remove(c);
            rename("temp.txt", c);
        }
        cout << " rows deleted" << endl;
    }
}
void tableno(map<string, int>& table, vector<string> schema)
{
    int count = 0;
    for(int j = 1; j < schema.size(); j+=2)
    {
        table[schema[j]] = count;
        count++;
    }
}
void checkcmd(map<int, string>& column_change, vector<string> query, vector<string> schema)
{
    int i;
    map<string, int> table;
    tableno(table, schema);
    for(i=3; i<query.size() && query[i] != "where"; i+=3)
    {
        int col = table[query[i]];
        column_change[col] = query[i+2];
    }
}

void update(vector<string> query)
{

    if(!isTable(query[1])){
        cout<<"Table does not exist"<<endl;
        return;
    }

    vector<string> schema;
    fetchSchema(query[1], schema);

    if(!schema.empty()){
        string table_name = query[1] + ".txt";
        fstream table;
        const char *c = table_name.c_str();
        table.open(c, ios::in);
        vector<string>::iterator it;
        it = find(query.begin(), query.end(), "where");
        map<int, string> column_change;
        checkcmd(column_change, query, schema);

        string line;
        vector<string> lineVec;
        int i, j=0;
        int flag = 0;
        int count = 0;
        fstream temp;
        bool changed=false;
        temp.open("temp.txt", ios::out);

        if(it == query.end())
        {
            while(getline(table, line))
            {
                string tab = line;
                i=0;
                string curr = "";
                while(i<tab.length())
                {
                    if(tab[i] == '#')
                    {
                        lineVec.push_back(curr);
                        curr = "";
                    }
                    else
                    {
                        curr += tab[i];
                    }

                    i++;
                }

                lineVec.push_back(curr);

                string new_line = "";
                map<int, string>::iterator itr;
                for(itr = column_change.begin(); itr != column_change.end(); itr++)
                {
                    lineVec[itr->first] = itr->second;
                }

                for(i = 0; i < lineVec.size()-1; i++)
                {
                    new_line += lineVec[i];
                    new_line += "#";
                }

                new_line += lineVec[i];
                temp << new_line << endl;
                lineVec.clear();
            }
        }

        else
        {
            while (getline(table, line))
            {
                string tab = line;
                i=0;
                string curr = "";
                while(i<tab.length())
                {
                    if(tab[i] == '#')
                    {
                        lineVec.push_back(curr);
                        curr = "";
                    }
                    else
                    {
                        curr += tab[i];
                    }
                    i++;
                }

                lineVec.push_back(curr);

                int idx = 0;
                idx = it - query.begin();
                j=0;
                for(i=1; i<schema.size(); i+=2)
                    {
                    if(query[idx+1] == schema[i])
                    {

                        if(query[idx+2] == "=")
                        {
                            if(lineVec[j] == query[idx+3])
                            {
                                flag = 1;
                                count++;
                            }
                        }

                        if(query[idx+2] == ">")
                        {
                            if(lineVec[j] > query[idx+3])
                            {
                                flag = 1;
                                count++;
                            }
                        }

                        if(query[idx+2] == "<")
                        {
                            if(lineVec[j] < query[idx+3])
                            {
                                flag = 1;
                                count++;
                            }
                        }
                    }
                    j++;
                }

                if(flag != 1)
                {
                    temp << line << endl;
                    lineVec.clear();
                }

                else{
                    changed=true;
                    string new_line = "";
                    map<int, string>::iterator itr;
                    for(itr=column_change.begin(); itr!=column_change.end(); itr++){
                        lineVec[itr->first] = itr->second;
                    }

                    for(i = 0; i < lineVec.size()-1; i++)
                    {
                        new_line += lineVec[i];
                        new_line += "#";
                    }
                    new_line += lineVec[i];
                    temp << new_line << endl;
                    lineVec.clear();
                }

                flag = 0;
            }
        }

        table.close();
        temp.close();
        remove(c);
        rename("temp.txt", c);
        if(changed)
        {
            cout << "Content updated" << endl;
        }
        else
        {
            cout << "Content not found!!" <<endl;
        }
    }
}

void fetchTable(string& tableName,vector<string>& att, unordered_map<string,int>& mp, vector<vector<string>>& d)
{
    fstream table(tableName+".txt",ios::in);
    string line;

    cout << "\n";
    for(auto x:att)
        cout << x << "\t";
    cout << "\n\n";

    while(getline(table,line))
    {
        stringstream ss(line);
        vector<string> data;

        while (ss.good())
        {
            string substr;
            getline(ss, substr, '#');
            data.push_back(substr);
        }

        d.push_back(data);

    }

}

void select(vector<string> &querytocons)
{
    string schema = "Schema.txt";
    // string tname = querytocons[querytocons.size() - 2];
    string tname = querytocons[3];

    //  cout<<tname<<endl;

    int fromindex = -99;
    for (int i = 0; i < querytocons.size(); i++)
    {
        if (querytocons[i] == "from")
        {
            fromindex = i;
            break;
        }
    }

    tname = querytocons[fromindex + 1];

    string requiredline;
    vector<string> tableinfo;
    map<string, int> collist;
    vector<string> colnamecheck;
    int whereindex = 0;

    vector<int> st;
    vector<string> selectedcolumns;

    if (!isTable(tname))
    {
        cout << "Table does not exists.." << endl;
        return;
    }
    else
    {
        tokenize(requiredline, "#", tableinfo);

        int cnt = 0;
        for (int i = 1; i < tableinfo.size(); i += 2)
        {
            collist[tableinfo[i]] = cnt;
            colnamecheck.push_back(tableinfo[i]);

            cnt++;
        }

        fstream table;
        string line;
        // ofstream fout;
        ifstream myfile(tname + ".txt");

        // check if where present

        for (int i = 0; i < querytocons.size(); i++)
        {
            if (querytocons[i] == "where")
            {
                whereindex = i;
                break;
            }
        }

        //  cout<<whereindex<<endl;
        if (myfile.is_open())
        {
            if (querytocons[1] == "*")
            {
                if (whereindex == 0)
                {
                    while (getline(myfile, line))
                    {
                        for (int i = 0; i < line.length(); i++)
                        {
                            if (line[i] == '#')
                                cout << " ";
                            else
                                cout << line[i];
                        }
                        cout << endl;
                    }
                }
                else if (whereindex == 4)
                {

                    while (getline(myfile, line))
                    {
                        vector<string> wherecondition;

                        tokenize(line, "#", wherecondition);

                        for (int i = 0; i < querytocons.size(); i++)
                        {

                            if (collist.find(querytocons[i]) != collist.end())
                            {

                                int pos = collist[querytocons[i]];
                                // cout<<pos<<" "<<querytocons[i+2]<<endl;
                                int flag = 0;

                                if (querytocons[i + 1] == "=")
                                {   
                                    
                                    if (wherecondition[pos + 1] == querytocons[i + 2])
                                        flag = 1;
                                }
                                else if (querytocons[i + 1] == ">")
                                {
                                    if (wherecondition[pos + 1] > querytocons[i + 2])
                                    {

                                        flag = 1;
                                    }
                                }
                                else if (querytocons[i + 1] == ">=")
                                {
                                    if (wherecondition[pos + 1] >= querytocons[i + 2])
                                        flag = 1;
                                }
                                else if (querytocons[i + 1] == "<")
                                {
                                    if (wherecondition[pos + 1] < querytocons[i + 2])
                                        flag = 1;
                                }
                                else if (querytocons[i + 1] == "<=")
                                {
                                    if (wherecondition[pos + 1] <= querytocons[i + 2])
                                        flag = 1;
                                }
                                else if (querytocons[i + 1] == "!=")
                                {
                                    if (wherecondition[pos + 1] != querytocons[i + 2])
                                        flag = 1;
                                }
                                else
                                    cout << "incorrect operator!!!!!!!!!!!" << endl;

                                if (flag)
                                {

                                    for (auto it : wherecondition)
                                    {
                                        cout << it << " ";
                                    }
                                    cout << endl;
                                }
                            //      else
                            // {
                            //     cout<<"Incorrect column name"<<endl;
                            //     return;
                            // }
                            }
                           
                        }
                    }
                }
                else
                {
                    cout << "ERROR IN WHERE" << endl;
                }
            }

            else
            {
                // for selected columns
                int ind = 1;
                while (querytocons[ind] != "from")
                {
                    if (querytocons[ind] != ",")
                    {
                        // cout<<querytocons[ind]<<" ";
                        selectedcolumns.push_back(querytocons[ind]);
                        int pos = collist[querytocons[ind]];
                        st.push_back(pos);
                    }

                    ind++;
                }

                for (auto it : collist)
                {
                    cout << it.first << " ";
                }
                cout << endl;
                cout << "**" << endl;
                for (auto it : selectedcolumns)
                {
                    vector<string>::iterator iter;
                    iter = find(colnamecheck.begin(), colnamecheck.end(), it);
                    if (iter == colnamecheck.end())
                    {
                        cout << "please add right col name" << endl;
                        return;
                    }

                    // cout<<it<<" ";
                }

                while (getline(myfile, line))
                {

                    vector<string> data;
                    // cout<<"hello"<<endl;
                    tokenize(line, "#", data);
                    //  for(auto it:data)
                    //  {
                    //      cout<<it<<" ";
                    //  }
                    for (auto it : st)
                    { // cout<<it<<" ";
                        cout << data[it + 1] << " ";
                    }
                    cout << endl;
                }
            }

            myfile.close();
        }

        else
            cout << "Unable to open file";
    }
}

void helpTable(){
    string line;

    schemafile.open("Schema.txt");

    int count = 0;
    while (getline(schemafile, line))
    {
        string l1 = line.substr(0, line.find('#'));
        cout << l1 << endl;
        count++;
    }
    if (!count)
    {
        cout << "No tables found" << endl;
    }
}

void helpCmd(vector<string> cmd){
    cout << "\n------------HELP----------------" << endl;
    map<string, int> help{{"createtable", 1}, {"droptable", 2}, {"insert", 3}};
    map<string, int>::iterator it1;
    string command = cmd[1] + (cmd.size() > 2 ? cmd[2] : "");
    it1 = help.find(command);

    switch (it1->second)
    {
    case 1:

        cout << "\nCommand : CREATE TABLE" << endl;
        cout << "Info: Creates a new table" << endl;
        cout << "\nFormat: \nCREATE TABLE table_name ( attribute_1 attribute1_type CHECK (constraint1), \nattribute_2 attribute2_type, …, PRIMARY KEY ( attribute_1, attribute_2 ), \nFOREIGN KEY ( attribute_y ) REFERENCES table_x ( attribute_t ), \nFOREIGN KEY ( attribute_w ) REFERENCES table_y ( attribute_z )...);" << endl;
        break;

    case 2:
        cout << "\nCommand : DROP TABLE" << endl;
        cout << "Info: Deletes a table" << endl;
        cout << "\nFormat: \nDROP TABLE table_name;" << endl;
        break;



    case 3:
        cout << "\nCommand : INSERT" << endl;
        cout << "Info: Inserts new data into a database" << endl;
        cout << "\nFormat: \nINSERT INTO table_name VALUES ( val1, val2, ... );" << endl;
        break;

    case 4:
        cout << "\nCommand : SELECT" << endl;
        cout << "Info: Extracts data from a database" << endl;
        cout << "\nFormat: \nSELECT attribute_list FROM table_list WHERE condition_list;" << endl;
        break;

    case 5:
        cout << "\nCommand : DELETE" << endl;
        cout << "Info: Deletes data from a database" << endl;
        cout << "\nFormat: \nDELETE FROM table_name WHERE condition_list;" << endl;
        break;

    case 6:
        cout << "\nCommand : UPDATE" << endl;
        cout << "Info: Updates data in a database" << endl;
        cout << "\nFormat: \nUPDATE table_name SET attr1 = val1, attr2 = val2… WHERE condition_list;" << endl;
        break;



    default:
        cout << "Syntax Error";
    }
}

void convertToVector(string input, vector<string> &v){
    string word = "";
    for (auto x : input)
    {
        if (x == ' ')
        {
            v.push_back(word);
            word = "";
        }
        else
        {
            word = word + x;
        }
    }
    v.push_back(word);
}

void fetchSchema(string tableName, vector<string> &schema){
    schemafile.open("Schema.txt", ios::in);
    string line;
    int flag = 0;

    while (getline(schemafile, line))
    {
        string l1 = line.substr(0, line.find('#'));
        if (tableName == l1)
        {
            flag = 1;
            stringstream ss(line);

            while (ss.good())
            {
                string substr;
                getline(ss, substr, '#');
                schema.push_back(substr);
            }
            break;
        }
    }
    if (flag == 0)
    {
        cout << "table not found" << endl;
    }

    schemafile.close();
}

void handleCmd(vector<string> cmd){
    if (cmd[0] == "create" && cmd[1] == "table")   //done
    {
        createTable(cmd);
    }
    else if (cmd[0] == "drop" && cmd[1] == "table") //done
    {
        dropTable(cmd);
    }
    else if (cmd[0] == "help" && cmd[1] == "table") //done
    {
        helpTable();
    }
    else if (cmd[0] == "help" && cmd[1] != "table") //done
    {
        helpCmd(cmd);
    }
    else if (cmd[0] == "insert" && cmd[1] == "into")  //done
    {
        insert(cmd);
    }
    else if (cmd[0] == "describe")   //done
    {
        describe(cmd);
    }

    else if (cmd[0] == "delete" && cmd[1] == "from")
    {
        delete_(cmd);
    }
     else if (cmd[0] == "update")
    {
        update(cmd);
    }
    else if (cmd[0] == "select")
    {
        select(cmd);
    }
    else if (cmd[0] == "truncate")
    {

        truncate(cmd);
    }

    else
    {
        cout << "Syntax Error";
    }
}

int main(){

    vector<string> cmd;
    string input;

    cout << "Group-60-DMS>";

    getline(cin, input);



    while (input != "q")
    {

        convertToVector(input, cmd);
        handleCmd(cmd);

        cmd.clear();
        cout << "\nGroup-60-DMS>";

        getline(cin, input);


    }

    return 0;
}
