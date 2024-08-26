#include <bits/stdc++.h>
#include <iostream>
using namespace std;
struct Instance
{
    map<string, string> attributes;
    string label;

};
struct DecisionTreeNode
{
    string feature;
    string label; 
    map<string, DecisionTreeNode *> children;
};
vector<Instance> loadDataFromCSV(const string &filename, vector<string> features)
{
    vector<Instance> data;
    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Error: Unable to open file " << filename << endl;
        return data;
    }
    string line;
    getline(file, line);
    while (getline(file, line))
    {
        istringstream iss(line);
        Instance instance;
        for (const string& attributeName : features)
        {
            string attributeValue;
            if (getline(iss, attributeValue, ','))
            {
                instance.attributes[attributeName] = attributeValue;
            }
        }
        string label;
        if (getline(iss, label, ','))
        {
            instance.label = label;
        }
        data.push_back(instance);
    }
    file.close();
    return data;
}
DecisionTreeNode *buildDecisionTree(const vector<Instance> &data, const vector<string> &features, int depth = 0)
{
    DecisionTreeNode *node = new DecisionTreeNode;
    if (depth >= 4 || data.size()<1)
    {
        map<string, int> labelCount;
        for (const Instance &instance : data)
        {
            labelCount[instance.label]++;
        }
        int maxCount = 0;
        for (const auto &entry : labelCount)
        {
            if (entry.second > maxCount)
            {
                maxCount = entry.second;
                node->label = entry.first;
            }
        }
        return node;
    }
    node->feature = features[depth % features.size()];
    map<string, vector<Instance>> dataSplit;
    for (const Instance &instance : data)
    {
        if (instance.attributes.find(node->feature) != instance.attributes.end())
        {
            const string &feature_value = instance.attributes.at(node->feature);
            dataSplit[feature_value].push_back(instance);
        }
    }
    for (const auto &entry : dataSplit)
    {
        node->children[entry.first] = buildDecisionTree(entry.second, features, depth + 1);
    }
    return node;
}
string classify(const Instance &instance, DecisionTreeNode *node)
{
    if (node->label != "")
    {
        return node->label;
    }
    string feature_value = "";
    if (instance.attributes.find(node->feature) != instance.attributes.end())
    {
        feature_value = instance.attributes.at(node->feature);
    }
    if (node->children.find(feature_value) == node->children.end())
    {
        // Handle missing data or unexpected feature values
        return "Unknown";
    }
    return classify(instance, node->children[feature_value]);
}
map<string, vector<string>> collectValidOptions(DecisionTreeNode *node,vector<string> features)
{
    map<string, set<string>> validOptions; // Use a set instead of a vector
    for(int c=0;c<features.size();c++)
    {
        if (node->feature == features[c]) {
            for (const auto &child: node->children) {
                validOptions[node->feature].insert(child.first); // Use insert instead of push_back
            }
        }
    }
    for (const auto &child : node->children)
    {
        const auto childOptions = collectValidOptions(child.second,features);
        for (const auto &option : childOptions)
        {
            validOptions[option.first].insert(option.second.begin(), option.second.end()); // Use insert
        }
    }
    map<string, vector<string>> result;
    for (const auto &entry : validOptions)
    {
        result[entry.first] = vector<string>(entry.second.begin(), entry.second.end());
    }
    return result;
}
void saveDataToCSV(const string &filename, const vector<Instance> &data,vector<string> features)
{
    ofstream file(filename);
    if (!file.is_open())
    {
        cerr << "Error: Unable to open file " << filename << " for writing" << endl;
        return;
    }
    for (const Instance &instance : data)
    {
        for(int j=0;j<features.size();j++)
        {
            file<< instance.attributes.at(features[j])<<",";
        }
        file<< instance.label << endl;
    }
    file.close();
}
void quiz(DecisionTreeNode *root, vector<string> features,vector<Instance> data1)
{
    DecisionTreeNode *currentNode = root;
    map<string, vector<string>> validOptions = collectValidOptions(root,features);
    int c = 0;
    while(currentNode->label.empty())
    {
        if (currentNode->feature == features[c])
        {
            cout << "Q. "<<features[c] << " index\n";
            for (size_t i = 0; i < validOptions[features[c]].size(); i++)
            {
                cout << i + 1 << ". " << validOptions[features[c]][i]<<endl;
            }
        }
        int option;
        cin >> option;
        if (option < 1 || option > static_cast<int>(validOptions[currentNode->feature].size()))
        {
            cout << "Enter valid input\n";
        }
        string feature=validOptions[currentNode->feature][option - 1];
        if (currentNode->children.find(feature) == currentNode->children.end())
        {
            cout << "Invalid option/No such person present in database with inputted features.\n";
            int options;
            cout << "Do you want to enter the celebrity guessed by you in the dataset?\n1. Yes\n2. No\n";
            cin >> options;
            if (options == 1) {
                // Collect attribute values from the user
                Instance newCelebrity;
                for (int i = 0; i < features.size(); i++) {
                    string attributeName = features[i];
                    cout << "Enter " << attributeName << ": ";
                    cin >> newCelebrity.attributes[attributeName];
                }
                // Collect the label (celebrity's name)
                cout << "Enter the celebrity's name: ";
                cin >> newCelebrity.label;
                // Add the new celebrity instance to the dataset
                data1.push_back(newCelebrity);
                // Save the updated dataset to the CSV file
                saveDataToCSV("indian_celebrities.csv", data1, features);
                int choice;
                cout<<"Do you want to continue?\n1. Yes\n2. No\n";
                cin>>choice;
                if(choice==1)
                {
                    quiz(root,features,data1);
                }
                else if(choice==2)
                {
                    cout<<"Thank you\nHope you have enjoyed our services\n";
                }
            } 
            else {
                continue;
            }
        }
        currentNode = currentNode->children[feature];
        c++;
    }
    cout << "Predicted Celebrity: " << currentNode->label << endl;
}
int main()
{
    cout<<"***************************************************************\n";
    cout<<"                      WELCOME TO PREDINATOR                    \n";
    cout<<"***************************************************************\n";
    cout<<"Here, we predict the name of celebrity guessed by you\nCurrently, our project is limited due to less data\n";
    int choice;
    while(true)
    {
        vector<string> features = {"Popularity", "Profession", "Education","Marital_Status"};
        vector<Instance> data1 = loadDataFromCSV("Predanator_database.csv", features);
        DecisionTreeNode *root = buildDecisionTree(data1, features);
        cout<<"1. View test results (2)\n2. Predict celebrity\n3. Exit\n";
        cin>>choice;
        if(choice==1)
        {
            Instance testInstance1 = {{{"Popularity", "Medium"}, {"Profession", "Politician"}, {"Education","Bachelor's_degree"},{"Marital_status","Married"}},""};
            string result1 = classify(testInstance1, root);
            cout << "Predicted Celebrity: " <<result1<<endl;
            Instance testInstance2 = {{{"Popularity", "Low"}, {"Profession", "F1driver"}, {"Education","College_Degree"},{"Marital_status","Married"}},""};
            string result2 = classify(testInstance2, root);
            cout << "Predicted Celebrity: " <<result2<<endl;
        }
        else if(choice==2)
        {
            cout<<"Answer the following:\n";
            quiz(root, features,data1);
        }
        else if(choice==3)
        {
            cout<<"Thank you\nHope you have enjoyed our services\n";
            break;
        }
        else cout<<"Enter valid choice\n";
    }
}
