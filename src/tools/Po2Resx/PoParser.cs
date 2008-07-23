using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace Po2Resx
{
	class Msg
	{
		readonly string _id;
		readonly string _value;
		readonly string _comment;

		public Msg(string id, string value, string comment)
		{
			_id = id;
			_value = value;
			_comment = comment;
		}

		public string Key
		{
			get { return _id; }
		}

		public string Value
		{
			get { return _value; }
		}

		public string Comment
		{
			get { return _comment; }
		}
	}

	class Token
	{
		string _key;
		string _value;
		string _comment;

		public Token(string key, string value, string comment)
		{
			_key = key;
			_value = value;
			_comment = comment;
		}

		public string Key
		{
			get { return _key; }
		}

		public string Value
		{
			get { return _value; }
		}

		public string Comment
		{
			get { return _comment; }
		}

	}

	class PoParser
	{
		public static IEnumerable<Msg> ReadMessages(FileInfo file)
		{
			Token msgid;
			Encoding encoding = null;
			bool again;

			do
			{
				msgid = null;
				again = false;

				foreach (Token token in ReadTokens(file, encoding ?? Encoding.ASCII))
				{
					switch (token.Key)
					{
						case "msgid":
							msgid = token;
							break;
						case "msgstr":
							if (msgid == null)
								throw new InvalidOperationException();

							if (msgid.Value == "")
							{
								if (encoding == null)
								{
									foreach (string line in token.Value.Split('\n'))
									{
										string l = line.Trim(); // Trim whitespace and '\r'

										if (l.StartsWith("Content-Type:", StringComparison.OrdinalIgnoreCase))
										{
											l = l.Substring(l.IndexOf(':') + 1).Trim();
											int c = l.IndexOf("CHARSET=", StringComparison.OrdinalIgnoreCase);

											if (c >= 0)
											{
												string cc = l.Substring(l.IndexOf('=', c) + 1);

												if (cc.Equals("UTF-8", StringComparison.OrdinalIgnoreCase) || cc.Equals("UTF8", StringComparison.OrdinalIgnoreCase))
													encoding = Encoding.UTF8;
												else if (cc.Equals("UTF-7", StringComparison.OrdinalIgnoreCase) || cc.Equals("UTF7", StringComparison.OrdinalIgnoreCase))
													encoding = Encoding.UTF7;
												else
													try
													{
														encoding = Encoding.GetEncoding(cc);
													}
													catch
													{
														Console.Error.WriteLine("warning: Ignoring unknown encoding '{0}'", cc);
														continue;
													}

												again = true;
												break;
											}
										}
									}
								}
							}
							else
								yield return new Msg(msgid.Value, token.Value, msgid.Comment ?? token.Comment);

							msgid = null;
							break;
						default:
							throw new InvalidOperationException("Unexpected token: " + token.Key);
					}

					if (again)
						break;
				}
			}
			while (again);

			if (msgid != null)
				throw new InvalidOperationException("MsgId without value: " + msgid);


		}

		private static IEnumerable<Token> ReadTokens(FileInfo file, Encoding encoding)
		{
			using (StreamReader sr = (encoding != null) ? new StreamReader(file.FullName, encoding) : new StreamReader(file.FullName))
			{
				string line;
				StringBuilder sbToken = new StringBuilder();
				StringBuilder sbTextBuilding = new StringBuilder();
				string comment = null;
				bool inString = false;
				bool inToken = false;

				while (null != (line = sr.ReadLine()))
				{
					if (!inString)
					{
						line = line.TrimStart();

						if (line.StartsWith("#", StringComparison.Ordinal))
						{
							if (line.StartsWith("#:"))
								comment = line.Substring(2);

							continue; // Skip line
						}
					}
					else
						sbTextBuilding.Append('\n');

					int i = 0;
					while (i < line.Length)
					{
						if (inString)
						{
							switch (line[i])
							{
								case '\\':
									if (++i >= line.Length)
										break; // Append newline on next line
									switch (line[i])
									{
										case 'a':
											sbTextBuilding.Append('\a');
											break;
										case 'b':
											sbTextBuilding.Append('\b');
											break;
										case 'f':
											sbTextBuilding.Append('\f');
											break;
										case 'n':
											sbTextBuilding.Append('\n');
											break;
										case 'r':
											sbTextBuilding.Append('\r');
											break;
										case 't':
											sbTextBuilding.Append('\t');
											break;
										case 'v':
											sbTextBuilding.Append('\v');
											break;
										case '\'':
										case '\"':
										case '?':
											sbTextBuilding.Append(line[i]);
											break;

										case '0':
										case '1':
										case '2':
										case '3':
										case '4':
										case '5':
										case '6':
										case '7':
										// Octal number
										case 'x':
											throw new NotImplementedException("Character c escapes not implemented yet");
									}
									break;
								case '"':
									inString = false;
									break;
								default:
									sbTextBuilding.Append(line[i]);
									break;
							}
						}
						else if (line[i] == '\"')
						{
							inString = true;
							inToken = false;
						}
						else if (char.IsWhiteSpace(line, i))
						{
							inToken = false;
						}
						else
						{
							if (!inToken && sbToken.Length > 0)
							{
								yield return new Token(sbToken.ToString(), sbTextBuilding.ToString(), comment);
								comment = null;

								sbToken.Length = 0;
								sbTextBuilding.Length = 0;

							}

							sbToken.Append(line[i]);
							inToken = true;
						}
						i++;
					}
				}

				if (!inToken && sbToken.Length > 0)
					yield return new Token(sbToken.ToString(), sbTextBuilding.ToString(), comment);
			}
		}
	}
}
