# Conventions

Below are the conventions of spock engine. 

Do's:
```cpp

// use snake case for every newly created variable, function, method, and etc. name
sushi_t i_love_sushi_t{};

// all variable types, class names, struct names, function ptr types are labled with _t
struct struct_name_t {};

class class_name_t {};

typedef uint32_t u32_t;

typedef std::function<void(int cool_paremeter)> function_t;

// for long named variable types use auto
auto my_var = a_really_long_name<with_c_plus_plus_templates_too>();

// when many variables declarations are next to eachtoher
// indent them in such a way that their '=' sign all on the same column:
int                  a_var = 0;
uint32_t  a_secondary_name = 4096;
int32_t  third_name_in_row = -1;

// opposite of this would look like (which is NOT wanted):
int a_var = 0;
uint32_t a_secondary_name = 4096;
int32_t third_name_in_row = -1;

// if the name of one variable declaration is too long to reasonably read then dont indent:
int a_nice_name = 0;
std::vector<really_long_typename_t>::iterator long_iterator_name;

// function & method naming
void noun_verb();

// e.g.
void tile_get();
void music_play();

// creation of a structure via a function thats not a method (procedual creation)

void font_create();
void font_destroy();

// avoid using:
void font_make();
void font_init(); // init word is only used with in classes

```

*Do not's*:
```cpp

// work in progress

```
