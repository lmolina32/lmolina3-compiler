# Name: Leonardo Molina
# NetID: lmolina3

## Encoder 
### Interpretation 
- For encode the values outside the ascii range (e.g not between 32 and 127) will be encoded with the value itself. This means that if the incoming string "\0xff" the decoder will replace the string with the ascii value and the encoder will not encode it back into '0xff' rather it would leave it. This is the interpretation of not turning ascii values outside of the range of 32-127 into hex.

### development encoder 
* What AI tools (if any) did you use, and what sort of prompts did you provide?
    - did not use any AI tools writing the code itself, used AI for code review 
    - Used (gpt-5) for code review: 
        - (gpt-5 ) - The code snippet I am given you is the source file for an encoder it has three functions, encode, decode char_to_hex. Its purpose is to encode and decode a programming language with specific specs. The code review should focus on useful comments, error handling and overall structure of the code. If there are any issues with the code e.g potential of a null pointer or improper error handling provide a suggestion on how to implement. Additionally, if code can be simplified provide this suggestion as well. Lastly some other areas it should focus on improving are suggestions on potential bugs, maintainability of the code, security and performance. If you implement the suggestions directly in the code base put them in between tags like this <code> <\code>. First think about how the two functions and helper functions work together. Then think about all the potential issues/improvements you can make on the code (e.g performance and readability) then think on these improvements for 3 minutes. If you have not thought of great suggestions think more until you have improved the functions without taking their purpose away from it. (Then attached encoder.c)
    - Implemented suggestions on combining if statements, simplifying logic, and produce error print statements. Other suggestions were ignored e.g using specific error codes when returning instead of just returning false and using a hash table instead of switch statements. 
* What parts of the code were easy to get right?
    - Main - For the main file what was easy to get write was reading in the file and calling the separate functions. Additionally, taking arguments through the command line and then parsing it was fairly straight forward.
    - encoder.c - The encoder was more straight forward than the decoder was. Maybe this was because I tackled the decoder first, but the encoder did not seem as difficult as the decoder nearly was. I think this is because all the edge case testing mainly occurs in the decoder not the encoder.
* What parts were difficult to get right and required more effort?
    - **Edge Cases** - What was hard to get right was the decoder by far. The decoder which initially seemed like an easy task quickly became challenging. Not because of the code itself (besides the hex conversion, initially) but due to edge cases. It became more apparent how strict the encoder needed to be once I started building the decoder. 
    - **testing** - Overlooked part for most of my classes since typically the test cases are built for me. However, building the test cases myself forced me to think hard on the language specifics but also on how an end-user would break my decoder/encoder. It was by far the hardest task of the assignment and still needs tons to develop before my test cases have 100% coverage on my code. 