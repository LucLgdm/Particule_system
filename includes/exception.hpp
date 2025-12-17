/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exception.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 11:09:53 by lde-merc          #+#    #+#             */
/*   Updated: 2025/12/17 15:02:49 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <exception>
#include <string>
using namespace std;

class inputError : public exception {
	public:
		explicit inputError(const std::string& m) : _msg(m) {}
		const char* what() const noexcept override { return _msg.c_str(); }
	private :
		string _msg;
};

class glfwError : public exception {
	public:
		explicit glfwError(const std::string& m) : _msg(m) {}
		const char* what() const noexcept override { return _msg.c_str(); }
	private :
		string _msg;
};

class openGlError : public exception {
	public:
		explicit openGlError(const std::string& m) : _msg(m) {}
		const char* what() const noexcept override { return _msg.c_str(); }
	private :
		string _msg;
};
