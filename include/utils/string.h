#pragma once

#include <algorithm>
#include <string>

#include <boost/algorithm/string.hpp>

namespace parallel_db::utils
{
inline std::string copyRemoveWhitespaces( const std::string& string )
{
    std::string result;
    std::unique_copy( string.begin(),
                      string.end(),
                      std::back_insert_iterator< std::string >( result ),
                      []( char a, char b )
                      {
                          return std::isspace( a ) && std::isspace( b );
                      } );

    return result;
}

inline std::string copyRemoveSemicolon( const std::string& string )
{
    std::string result{ string };
    boost::erase_all( result, ";" );
    return result;
}
} // namespace parallel_db::utils