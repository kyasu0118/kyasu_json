//  Created by kyasu on 2018/11/11.
//  Copyright 2018 kyasu. All rights reserved.

#ifndef kyasu_json_h
#define kyasu_json_h

#include <exception>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <map>

namespace kyasu
{
    class json
    {
        friend int operator>>( const std::string& text, json& json);
        friend std::ostream& operator<<(std::ostream& os, const json& json);
    private:
        enum class Type { None, Value, Array, Object };
        Type type = Type::None;
        std::string value;
        std::vector< json > array;
        std::map< std::string, json > object;
    public:
        operator double() const{ return atof( value.c_str() ); }
        operator int() const{ return atoi( value.c_str() ); }
        operator std::string() const { return value.substr( 1, value.size() - 2 ); }
        operator bool() const
        {
            if( value == "true" ){ return true; }
            else if( value == "false" ){ return false; }
            throw std::exception();
        }
        const json& operator[]( int index ) const { return array[index]; }
        
        json& operator[]( int index )
        {
            type = Type::Array;
            if( index >= array.size() )
            {
                array.resize( index + 1 );
            }
            return array[index];
        }

        const json& operator[]( const char* key ) const
        {
            return object.at( key );
        }

        json& operator[]( const char* key )
        {
            type = Type::Object;
            return object[key];
        }

        json& operator=( double value )
        {
            type = Type::Value;
            this->value = (std::stringstream() << value).str();
            return *this;
        }

        json& operator=( int value )
        {
            type = Type::Value;
            this->value = (std::stringstream() << value).str();
            return *this;
        }

        json& operator=( const char* value )
        {
            type = Type::Value;
            this->value = std::string("\"") + value + '\"';
            return *this;
        }

        json& operator=( const std::string& value )
        {
            type = Type::Value;
            this->value = '\"' + value + '\"';
            return *this;
        }

        json& operator=( bool value )
        {
            type = Type::Value;
            if( value == true )
            {
                this->value = "true";
            }
            else
            {
                this->value = "false";
            }
            return *this;
        }
    };

    std::ostream& operator<<(std::ostream& os, const json& json)
    {
        bool first = true;

        if( json.type == json::Type::Value )
        {
            os << json.value;
        }
        else if( json.type == json::Type::Array )
        {
            os << "[";
            for( const auto& i : json.array )
            {
                if( first )
                {
                    first = false;
                }
                else
                {
                    os << ",";
                }
                os << i;
            }
            os << "]";
        }
        else
        {
            os << "{";
            for( const auto& i : json.object )
            {
                if( first )
                {
                    first = false;
                }
                else
                {
                    os << ",";
                }
                os << "\"" << i.first << "\":" << i.second;
            }
            os << "}";
        }
        return os;
    }
    
    int operator>>( const std::string& text, json& json )
    {
        auto skip = []( const std::string& text, int& i)
        {
            while( text[i] == ' ' || text[i] == '\r' || text[i] == '\n' || text[i] == '\t' )
            {
                ++i;
            }
        };
        
        int begin;
        for( int i=0; i<text.size(); ++i )
        {
            skip( text, i );
            if( text[i] == '{' )
            {
                do
                {
                    skip( text, ++i );
                    
                    if( text[i++] != '"' )
                    {
                        throw std::exception();
                    }
                    begin = i;
                    while( text[i] != '"' )
                    {
                        ++i;
                    }
                    const std::string key = text.substr( begin, i-begin );

                    skip( text, ++i );                    
                    if( text[i++] != ':' )
                    {
                        throw std::exception();
                    }
                    
                    skip( text, i );
                    
                    if( text[i] == '{' )
                    {
                        &text[i] >> json.object[key];
                    }
                    else
                    {
                        begin = i;
                        while( text[i] != ',' && text[i] != '}' )
                        {
                            ++i;
                        }
                        const std::string value = text.substr( begin, i-begin );

                        json[key.c_str()].type = json::Type::Value;
                        json[key.c_str()].value = value;
                        
                        skip( text, i );
                    }
                }while( text[i] == ',' );
                ++i;
            }
            else if( text[i] == '[' )
            {
                int index = 0;
                do
                {
                    skip( text, ++i );
                    
                    if( text[i] == '{' )
                    {
                        i += &text[i] >> json[index];
                    }
                    else if( text[i] == '[' )
                    {
                        i += &text[i] >> json[index];
                    }
                    else if( text[i] == ']' )
                    {
                        break;
                    }
                    else
                    {
                        begin = i;
                        while( text[i] != ',' && text[i] != ']' )
                        {
                            ++i;
                        }
                        const std::string value = text.substr( begin, i-begin );
                        
                        json[index] = value;
                        json.array[index].type = json::Type::Value;
                        ++index;
                    }
                }while( text[i] == ',' );
            }
        }
        return (int)text.size();
    }
}

#endif /* kyasu_json_h */
