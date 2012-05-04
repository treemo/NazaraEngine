// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FILE_HPP
#define NAZARA_FILE_HPP

#define NAZARA_FILE

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Directory.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <Nazara/Core/Hashable.hpp>
#include <Nazara/Core/HashDigest.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Core/ThreadSafety.hpp>
#include <Nazara/Utility/NonCopyable.hpp>

class NzFileImpl;

class NAZARA_API NzFile : public NzHashable, NzNonCopyable
{
	public:
		enum CursorPosition
		{
			AtBegin,   // D�but du fichier
			AtCurrent, // Position du pointeur
			AtEnd	   // Fin du fichier
		};

		enum OpenMode
		{
			Current   = 0x00, // Utilise le mode d'ouverture actuel

			Append	  = 0x01, // Emp�che l'�criture sur la partie d�j� existante et met le curseur � la fin
			Lock	  = 0x02, // Emp�che le fichier d'�tre modifi� tant qu'il est ouvert
			ReadOnly  = 0x04, // Ouvre uniquement en lecture
			ReadWrite = 0x08, // Ouvre en lecture/�criture
			Text	  = 0x10, // Ouvre en mode texte
			Truncate  = 0x20, // Cr�� le fichier s'il n'existe pas et le vide s'il existe
			WriteOnly = 0x40  // Ouvre uniquement en �criture, cr�� le fichier s'il n'existe pas
		};

		NzFile();
		NzFile(const NzString& filePath);
		NzFile(const NzString& filePath, unsigned long openMode);
		NzFile(NzFile&& file);
		~NzFile();

		bool Copy(const NzString& newFilePath);
		void Close();

		bool Delete();

		bool EndOfFile() const;

		bool Exists() const;

		void Flush();

		time_t GetCreationTime() const;
		nzUInt64 GetCursorPos() const;
		NzString GetDirectoryPath() const;
		NzString GetFilePath() const;
		NzString GetFileName() const;
		time_t GetLastAccessTime() const;
		time_t GetLastWriteTime() const;
		NzString GetLine(unsigned int lineSize = 0);
		nzUInt64 GetSize() const;

		bool IsOpen() const;

		bool Open(unsigned long openMode = Current);

		std::size_t Read(void* buffer, std::size_t typeSize, unsigned int count);
		bool Rename(const NzString& newFilePath);

		bool SetCursorPos(CursorPosition pos, nzInt64 offset = 0);
		bool SetCursorPos(nzUInt64 offset);
		void SetEndianness(nzEndianness endianness);
		bool SetFile(const NzString& filePath);
		bool SetOpenMode(unsigned int openMode);

		bool Write(const NzString& string);
		std::size_t Write(const void* buffer, std::size_t typeSize, unsigned int count);

		NzFile& operator=(const NzString& filePath);
		NzFile& operator=(NzFile&& file);

		static NzString AbsolutePath(const NzString& filePath);
		static bool Copy(const NzString& sourcePath, const NzString& targetPath);
		static bool Delete(const NzString& filePath);
		static bool Exists(const NzString& filePath);
		static time_t GetCreationTime(const NzString& filePath);
		static time_t GetLastAccessTime(const NzString& filePath);
		static time_t GetLastWriteTime(const NzString& filePath);
		static NzHashDigest GetHash(const NzString& filePath, nzHash hash);
		static NzHashDigest GetHash(const NzString& filePath, NzHashImpl* hash);
		static nzUInt64 GetSize(const NzString& filePath);
		static bool IsAbsolute(const NzString& path);
		static NzString NormalizePath(const NzString& filePath);
		static NzString NormalizeSeparators(const NzString& filePath);
		static bool Rename(const NzString& sourcePath, const NzString& targetPath);

	private:
		bool FillHash(NzHashImpl* hash) const;

		NazaraMutexAttrib(m_mutex, mutable)

		nzEndianness m_endianness;
		NzString m_filePath;
		NzFileImpl* m_impl;
		unsigned int m_openMode;
};

#undef NAZARA_FILE

#endif // NAZARA_FILE_HPP