# Read Me
Implementated a fast version of the autocomplete functionality. Worked with files that contain a large number of terms, along with the importance weight
associated with those terms. The autocomplete task is to quickly retrieve the top terms that match a query string. For example, if the query string is "Eng", your matches might be "English", "Engineering", and "EngSci". In the project, matches are case-sensitive, and only the beginning of the string is matched. "Eng" matches "EngSci" but not "engaged" and "Sci" matches "Science" but not "EngSci".
To accomplish the task, you will:
- Read in the entire file of terms, and sort the terms in lexicographic ordering. This sorted list will be reused for multiple queries
- Use binary search to find the location of the first term in lexicographic ordering that matches the query string, as well as the last term in lexicographic ordering that matches the query string
- Extract the terms that match the query string, and sort them by weight
- Extract the top matching terms by weight
