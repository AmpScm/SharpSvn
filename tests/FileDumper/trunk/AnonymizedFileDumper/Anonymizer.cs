using System;
using System.Collections.Generic;
using System.IO;
using System.Security.Cryptography;
using System.Text;

class Anonymizer : Stream
{
    readonly Stream _inner;
    readonly HashAlgorithm _hash;
    readonly StreamWriter _writer;

    public Anonymizer(Stream toStream)
    {
        _inner = toStream;
        _hash = new Crc32();
        _writer = new StreamWriter(_inner);
    }

    public override bool CanRead
    {
        get { return false; }
    }

    public override bool CanSeek
    {
        get { return false; }
    }

    public override bool CanWrite
    {
        get { return false; }
    }

    public override void Flush()
    {
    }

    public override void Close()
    {
        PushLine();
        base.Close();
    }

    public override long Length
    {
        get { return -1; }
    }

    public override long Position
    {
        get
        {
            return -1;
        }
        set
        {
            throw new InvalidOperationException();
        }
    }

    public override int Read(byte[] buffer, int offset, int count)
    {
        throw new NotImplementedException();
    }

    public override long Seek(long offset, SeekOrigin origin)
    {
        throw new NotImplementedException();
    }

    public override void SetLength(long value)
    {
        throw new NotImplementedException();
    }

    readonly List<byte> _lineBuffer = new List<byte>();
    enum LinePosition
    {
        Start,
        Body,
    }

    LinePosition _pos;

    public override void Write(byte[] buffer, int offset, int count)
    {
        while (count > 0)
        {
            if (_pos == LinePosition.Start && IsWhite(buffer, offset))
            {
                _inner.WriteByte(buffer[offset]);
                count--;
                offset++;
                continue;
            }
            _pos = LinePosition.Body;
            if (buffer[offset] == '\n')
            {
                PushLine();
                _inner.WriteByte((byte)'\n');
                _pos = LinePosition.Start;
            }
            else
            {
                _lineBuffer.Add(buffer[offset]);
            }
            offset++;
            count--;
        }
    }

    private void PushLine()
    {
        int nSpace = 0; // Nr of whitespace characters at end
        while (nSpace < _lineBuffer.Count && IsWhite(_lineBuffer, _lineBuffer.Count - nSpace - 1))
            nSpace++;

        _writer.Write(CalcHash(_lineBuffer, 0, _lineBuffer.Count - nSpace));
        _writer.Flush();
        if (nSpace > 0)
            _inner.Write(_lineBuffer.ToArray(), _lineBuffer.Count - nSpace, nSpace);
        _lineBuffer.Clear();
    }

    private string CalcHash(List<byte> buffer, int offset, int count)
    {
        if (count > 0)
        {
            StringBuilder sb = new StringBuilder();
            foreach (byte b in _hash.ComputeHash(buffer.ToArray(), offset, count))
            {
                sb.AppendFormat("{0:X2}", b);
            }
            return sb.ToString();
        }
        else
            return "";
    }

    private bool IsWhite(IList<byte> buffer, int offset)
    {
        char c = (char)buffer[offset];

        return c != '\n' && Char.IsWhiteSpace(c);
    }
}
