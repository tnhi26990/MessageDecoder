#include <algorithm>
#include <bits/stdc++.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

// creates a vector that pairs each letter in the cipher to the corresponding
// english letter based on letter frequency
vector<pair<char, char>>
countEnglishFrequency(vector<pair<char, int>> &sortedFrequency) {
  vector<pair<char, char>> englishFrequency;
  char letterFrequency[26] = {'E', 'T', 'A', 'O', 'I', 'N', 'S', 'H', 'R',
                              'D', 'L', 'C', 'U', 'M', 'W', 'F', 'G', 'Y',
                              'P', 'B', 'V', 'K', 'J', 'X', 'Q', 'Z'};
  englishFrequency.resize(
      sortedFrequency
          .size()); // ensures no runtime errors due to varying sizes of vectors
  for (int i = 0; i < sortedFrequency.size(); i++) {
    englishFrequency[i].first = sortedFrequency[i].first;
    englishFrequency[i].second = letterFrequency[i];
  }
  return englishFrequency;
}

// calculates each character and the amount of character occurrences in the
// string
unordered_map<char, int> countCharacter(string &ciphertext) {
  unordered_map<char, int> characterCount;
  for (char c : ciphertext) {
    if (isalpha(c)) {
      characterCount[c]++;
    }
  }
  return characterCount;
}

// mirrors the contents of the cipher.txt to a string variable
string txtToString() {
  string encryptedCipher;
  string myCipher;
  ifstream cipherFile;
  cipherFile.open("ciphertext.txt");

  if (cipherFile.is_open()) {
    while (getline(cipherFile, myCipher)) {
      encryptedCipher += myCipher;
    }
  }
  cipherFile.close();
  return encryptedCipher;
}

// mirrors the contents of the dictionary.txt to a vector variable
vector<string> txtToVector() {
  vector<string> dictionary;
  string myDict;
  ifstream dictionaryFile;
  dictionaryFile.open("dictionary.txt");
  if (dictionaryFile.is_open()) {
    while (getline(dictionaryFile, myDict)) {
      dictionary.push_back(myDict);
    }
  }
  return dictionary;
}

// decrypts the cipher string by replacing each character based on it's english
// frequency corresponding character and returns a decrypted string
string decryptCipher(vector<pair<char, char>> &englishFrequency) {
  string eCipher = txtToString();
  string decryptedCipher;

  for (char &c : eCipher) {
    for (auto &engPair : englishFrequency) {
      char decryptLetter = engPair.first;
      char encryptLetter = engPair.second;

      if (c == decryptLetter) {
        decryptedCipher += encryptLetter;
      }
    }
  }
  return decryptedCipher;
}

// counts the amount that each word, for our purposes in the dictionary, appear
// in a string
int countWordFrequency(string &searchWord, string &cipher) {
  int searchWordLength = searchWord.length();
  int cipherLength = cipher.length();
  int wordFrequency = 0;

  for (int i = 0; i <= cipherLength - searchWordLength; i++) {
    int j;

    for (j = 0; j < searchWordLength; j++)
      if (cipher[i + j] != searchWord[j]) {
        break;
      }
    if (j == searchWordLength) {
      wordFrequency++;
    }
  }
  return wordFrequency;
}

// calculates the amount of valid words are in the cipher
int validWordCount(string &cipher, vector<string> &dictionary) {
  int wordCount = 0;
  for (auto &word : dictionary) {
    if (cipher.find(word)) {
      wordCount += countWordFrequency(word, cipher);
    }
  }
  return wordCount;
}

//according to what character the analyst wants swap, updates the key and cypher appropriately
vector<pair<char, char>> swapKeyCharacterByUser(vector<pair<char, char>> &key, char &characterToMap, char &characterToMapTo) {
  char justSwapped;
  char toReplace;
  for (auto &character : key) {
    if (character.second == characterToMap) {
      justSwapped = character.first;
      toReplace = character.second;
      character.second = toupper(characterToMapTo);
    }
  }
  for (auto &character : key) {
    if (character.second == characterToMapTo && character.first != justSwapped) {
      character.second = toupper(toReplace);
    }
  }

  return key;
}

//sorts based on value and converts from unordered map to vector
vector<pair<char, int>>
sortAndConvertToVector(unordered_map<char, int> &characterCount) {
  vector<pair<char, int>> sortedFrequency(characterCount.begin(),
                                          characterCount.end());
  std::sort(sortedFrequency.begin(), sortedFrequency.end(),
            [](const auto &left, const auto &right) {
              return left.second > right.second;
            });
  return sortedFrequency;
}

//programmatically permutes through key to yield as best a key and cypher to the analyst
vector<pair<char, char>> shiftKey(vector<pair<char, char>> &key, int count) {
  vector<string> dictionary = txtToVector();
  vector<pair<char, char>> tempKey = key;
  vector<pair<char, char>> bestKey = key;
  int bestScore = count;
  for (int i = 0; i < tempKey.size() - 1; i++) {
    for (int j = i + 1; j < tempKey.size(); j++) {
      std::swap(tempKey[i].second, tempKey[j].second);
      string cipher = decryptCipher(tempKey);
      int score = validWordCount(cipher, dictionary);
      if (score > bestScore) {
        bestScore = score;
        bestKey = tempKey;
      } else {
       std::swap(tempKey[i].second, tempKey[j].second);
      }
    }
  }
  return bestKey;
}


int main() {
  char swapFrom;
  char swapTo;
  bool improved = true;
  vector<string> dictionary = txtToVector();
  string cipherText = txtToString();
  unordered_map<char, int> cipherCharacterCount = countCharacter(cipherText);
  vector<pair<char, int>> sortedCipherVector = sortAndConvertToVector(cipherCharacterCount);
  vector<pair<char, char>> initialKey = countEnglishFrequency(sortedCipherVector);

  cout << '\n' << endl;
  cout << "The frequency analysis of the ciphertext: " << endl;
  for (auto &it : sortedCipherVector) {
    cout << it.first << ' ' << it.second << endl;
  }
  
  cout << '\n' << endl;
  cout << "The following is the key and the decrypted cipher after swapping according to English frequency. " << '\n';
  cout << '\n' << endl;
  cout << "Initial key: " << endl;
  for (auto &it : initialKey) {
    cout << it.first << ' ' << it.second << endl;
  }

  string decryptedCipher = decryptCipher(initialKey);
  cout << '\n' << endl;
  cout << "decrypted cipher: " << endl;
  cout << decryptedCipher << endl;

  int bestCount = validWordCount(decryptedCipher, dictionary);
  cout << '\n' << endl;
  cout << "valid word count: " << bestCount << endl;


  cout << '\n' << endl;
  cout << "The following is the best key and decrypted ciphertext after shifting programmatically. It may take the program a little more than one minute to print them to console because of the shiftKey() function." << '\n';
  vector<pair<char, char>> bestKey = shiftKey(initialKey, bestCount);
  cout << '\n' << endl;
  cout << "Best key: " << endl;
  for (auto &it : bestKey) {
    cout << it.first << ' ' << it.second << endl;
  }
  string dCipherBest = decryptCipher(bestKey);
  cout << '\n' << endl;
  cout << "decrypted cipher: " << endl;
  cout << dCipherBest << endl;
  
  int bestScore = validWordCount(dCipherBest, dictionary);
  cout << '\n' << endl;
  cout << "valid word count: " << bestScore << endl;

  cout << '\n' << endl;
  cout << "Now you can manually swap the rest. You can type upper-case or lower-case letters." << '\n';
  
  while (swapFrom != '!') {
    cout << "what cipher character would you like swapped? To end the session, type '!' " << '\n';
    cin >> swapFrom;

    if (swapFrom == '!') {
      cout << "This decrypting session is over. This is the final key and decrypted cipher." << '\n';
      break;
    }
    
    cout << "what character would you like it to map to?" << '\n';
    cin >> swapTo;
    char swapFromUpperCase = toupper(swapFrom);
    char swapToUpperCase = toupper(swapTo);
    vector<pair<char, char>> key = swapKeyCharacterByUser(bestKey, swapFromUpperCase, swapToUpperCase);
    cout << '\n' << endl;
    cout << "New key: " << endl;
    for (auto &it : key) {
      cout << it.first << ' ' << it.second << endl;
    }

    string dCipher = decryptCipher(key);
    cout << '\n' << endl;
    cout << "decrypted cipher: " << endl;
    cout << dCipher << endl;

    int bestCount = validWordCount(dCipher, dictionary);
    cout << '\n' << endl;
    cout << "valid word count: " << bestCount << endl;

    if (swapFrom == '!') {
      cout << "This decrypting session is over. This is the final key and decrypted cipher." << '\n';
      break;
    }
  }
  return 0;
}