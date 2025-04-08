# Thesis_ChoreographyTests

An aggregation of mock APIs representing faulty and correct implementations of a parameterized choreography describing communication from a `.cor` file. The program should be able to detect valid and invalid implementations.

Mock APIs and choreographies are based on documented examples in [ROS](https://github.com/ros/ros) and its extensions from [The Robust Project](https://github.com/robust-rosin/robust).

---

## Publisher_Subscriber

**Bug ID**: `0a76d5`

The [geometry2](https://github.com/ros/geometry2) extension to ROS has a bug in its API, where the parsing of a message sent to a ROS topic can be handled and modified concurrently by one or more nodelets (threads), creating a race condition.

---

## Driver_Nodelet

**Bug ID**: `0366a`

The [Kobuki_Melodic](https://github.com/yujinrobot/kobuki/) had a bug (fixed in SHA `03660afb5917366cc6d57567e2f695450fd95feb`), where nodelets (threads containing a controller and driver for a hardware component) could publish a topic and subscribe to other nodelets' topics. However, as no dependencies existed between two nodelets sending commands to each other, race conditions could occur. For example, the left and right bumpers on a vacuum robot could be activated at the same time, leading to incompatible rotate commands being sent simultaneously, causing unexpected behavior. In the actual Kobuki Robot, this was not an issue, as commands were dealt with synchronously.

---

## Choreography Language Design

The choreography language incorporates elements from [Carbone et al. 2016](https://dl.acm.org/doi/10.1145/2827695) while expanding the syntax with parametric types from [Deniélou et al. 2012](https://arxiv.org/pdf/1208.6483). 

To allow for the mapping of a parameterized type to C++ code, a declarative layer has been added to the language. Wrappers around different recursion approaches have also been introduced. Finally, as the project works with abstracted versions of APIs from the ROS operating system, an **arbitrary selection operator** has been implemented to abstract away the usage of a specific API.

---

## Choreography Language EBNF

The language is defined by the following EBNF:

```ebnf
<choreography> ::= <declaration>* <global_type_declaration>

<declaration> ::= <index_decl>
                | <participant_decl>
                | <channel_decl>
                | <label_decl>
                | <global_type_declaration>

<index_decl> ::= "Index" <identifier> "=" "{" <range> "}"
<range> ::= <number> | <number> ".." <number> | <number> ".." "n"

<participant_decl> ::= "Participant" <identifier>
                    |"Participant" <identifier> "{" <index_expr> "}"

<channel_decl> ::= "Channel" <identifier>
                 | "Channel" <identifier> "{" <index_expr> "}"

<label_decl> ::= "Label" <identifier> "{" <label_list> "}"
<label_list> ::= <identifier> ("," <identifier>)*

<global_type_declaration> ::= <identifier> "=" <global_type>

<global_type> ::= <interaction>
                | <recursion>
                | <branching>
                | <foreach>
                | <if_else>
                | <composition>
                | <end>

<interaction> ::= <participant> "->" <participant> ":" <channel> "<" <type> ">" ("{" <global_type> "}")?

<recursion> ::= "Rec" <identifier> "(" <index_binding> ")" "{" <global_type> "}"
<index_binding> ::= <identifier> ":"( <index_expr> | <type> ) ("," <identifier> ":" ( <index_expr> | <type> ))*

<foreach> ::= "foreach" "(" <identifier> ":" <index_expr> ")" "{" <global_type> "}"

<if_else> ::= "if" <condition> "then" "{" <global_type> "}" "else" "{" <global_type> "}"

<branching> ::= <label> "::" <identifier> ":" <interaction> "." <global_type>
              | <label> "::" <identifier> ":" <interaction> "|" <global_type>
              | <label> "::" <identifier> ":" <end>

<composition> ::= <linear_composition> | <parallel_composition>
<linear_composition> ::= <global_type> "." <global_type>
<parallel_composition> ::= <global_type> "|" <global_type>

<participant> ::= <identifier>
                | <identifier> "[" <index_expr> "]"

<channel> ::= <identifier>
            | <identifier> "[" <index_expr> "]"

<type> ::= "nat" | "bool" | <identifier>

<index_expr> ::= <identifier>
               | <number>
               | <number> ".." <number>

<label> ::= <identifier>

<condition> ::= <expression>
<expression> ::= <identifier> <operator> <value>
<value> ::= <number> | <identifier>

<comment> ::= "#" <text>