/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0.  If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright 1997 - July 2008 CWI, August 2008 - 2018 MonetDB B.V.
 */

package nl.cwi.monetdb.embedded.jdbc;

import nl.cwi.monetdb.embedded.env.MonetDBEmbeddedConnection;
import nl.cwi.monetdb.embedded.env.MonetDBEmbeddedDatabase;
import nl.cwi.monetdb.embedded.env.MonetDBEmbeddedException;
import nl.cwi.monetdb.jdbc.MonetConnection;
import nl.cwi.monetdb.jdbc.MonetStatement;
import nl.cwi.monetdb.mcl.connection.ControlCommands;
import nl.cwi.monetdb.mcl.connection.MCLException;
import nl.cwi.monetdb.mcl.protocol.ProtocolException;

import java.io.*;
import java.net.SocketException;
import java.sql.BatchUpdateException;
import java.sql.Connection;
import java.sql.SQLException;
import java.sql.SQLWarning;
import java.util.List;
import java.util.Properties;

/**
 * A {@link Connection} suitable for the MonetDB database using an embedded connection.
 *
 * @author <a href="mailto:pedro.ferreira@monetdbsolutions.com">Pedro Ferreira</a>
 */
public final class EmbeddedConnection extends MonetConnection {

	/** The directory of the database */
	private final String directory;

	/** If the sequential flag is set */
	private final boolean silentFlag;

	/** If the silent flag is set */
	private final boolean sequentialFlag;

	public EmbeddedConnection(Properties props, String hash, String language, String directory) {
		super(props, hash, EmbeddedLanguage.GetLanguageFromString(language), true, true);
		this.directory = directory;
		this.silentFlag = true;
		this.sequentialFlag = false;
	}

	public EmbeddedConnection(Properties props, String hash, String language, String directory, Boolean silentFlag,
							  Boolean sequentialFlag) {
		super(props, hash, EmbeddedLanguage.GetLanguageFromString(language), true, true);
		this.directory = directory;
		this.silentFlag = silentFlag;
		this.sequentialFlag = sequentialFlag;
	}

	/**
	 * Gets the directory where the database is hosted. Null pointer means is running in-memory.
	 *
	 * @return The directory where the database is hosted. Null pointer means is running in-memory
	 */
	public String getDirectory() {
		return directory;
	}

	/**
	 * Is the connection running in memory?
	 *
	 * @return Is the connection running in memory?
	 */
	public boolean isRunningInMemory() {
		return directory == null;
	}

	/**
	 * Is the silent flag set?
	 *
	 * @return Is the silent flag set?
	 */
	public boolean isSilentFlag() {
		return silentFlag;
	}

	/**
	 * Is the sequential flag set?
	 *
	 * @return Is the sequential flag set?
	 */
	public boolean isSequentialFlag() {
		return sequentialFlag;
	}

	/**
	 * Makes a conversion of this connection as a {@link MonetDBEmbeddedConnection} instance, making it possible to
	 * access the {@link MonetDBEmbeddedConnection} API on a JDBC embedded connection, although that API doesn't follow
	 * the JDBC specification.
	 *
	 * @return This connection as a {@link MonetDBEmbeddedConnection} instance
	 */
	public MonetDBEmbeddedConnection getAsMonetDBEmbeddedConnection() {
		return ((EmbeddedProtocol)protocol).getEmbeddedConnection();
	}

	/**
	 * Connects to the existing database on the JVM process. If the database is not running, then it will start.
	 * However if the database is already running in a different directory, a {@link MCLException} will be thrown.
	 * <br>
	 * At this moment, no user authentication is performed in an embedded connection, hence the username and password
	 * parameters can be ignored.
	 *
	 * @param user The user name to authenticate (ignored)
	 * @param pass The user's password (ignored)
	 * @return Always a null list
	 * @throws MCLException If the database is already running in a different directory
	 */
	@Override
	public List<String> connect(String user, String pass) throws IOException, ProtocolException, MCLException {
		try {
			this.protocol = new EmbeddedProtocol(MonetDBEmbeddedDatabase.createJDBCEmbeddedConnection(this.directory,
					this.silentFlag, this.sequentialFlag));
		} catch (MonetDBEmbeddedException ex) {
			throw new MCLException(ex);
		}
		return null;
	}

	/**
	 * Gets the underlying connection JDBC String URL.
	 *
	 * @return The underlying connection JDBC String URL
	 */
	@Override
	public String getJDBCURL() {
		if(this.directory != null) {
			return "jdbc:monetdb:embedded:" + this.directory;
		} else {
			return "jdbc:monetdb:embedded::memory:";
		}
	}

	/**
	 * Gets the underlying connection block size length. On an embedded connection, there is no boundary restrictions,
	 * so we always return the integer max value.
	 *
	 * @return The integer max value
	 */
	@Override
	public int getBlockSize() {
		return Integer.MAX_VALUE;
	}

	/**
	 * Gets the underlying connection default fetch size for DataBlock responses. On an embedded connection, there is no
	 * boundary restrictions, so we always return the integer max value.
	 *
	 * @return The integer max value
	 */
	@Override
	public int getDefFetchsize() {
		return Integer.MAX_VALUE;
	}

	@Override
	public int initialStringBuilderSize() {
		return 8 * 1024; //The same as the block size in a MAPI connection
	}

	/**
	 * In a embedded connection there is no timeout, so this methods always adds a {@link SQLWarning} and returns -1.
	 *
	 * @return -1
	 */
	@Override
	public int getSoTimeout() throws SocketException {
		throw new SocketException("Cannot get a timeout on a embedded connection");
	}

	/**
	 * In a embedded connection there is no timeout, so this methods always adds a {@link SQLWarning} and the timeout
	 * parameter is ignored.
	 *
	 * @param timeout The specified timeout, in milliseconds (ignored)
	 */
	@Override
	public void setSoTimeout(int timeout) throws SocketException {
		throw new SocketException("Cannot set a timeout on a embedded connection");
	}

	/**
	 * Closes the underlying connection implementation. On a embedded connection, after the connection is closed, if
	 * there are no more connections on the database, then the database is also shut down.
	 *
	 * @throws IOException if an error happens while closing the connection or the database
	 */
	@Override
	public synchronized void closeUnderlyingConnection() throws IOException {
		((EmbeddedProtocol)protocol).getEmbeddedConnection().close();
	}

	/**
	 * Sends a control command to the server. On an embedded connection, a specific call is performed for each command
	 * on the server.
	 *
	 * @param commandID the command identifier according to {@link ControlCommands} listing
	 * @param data The integer to send according to the control command
	 * @throws SQLException if an IO exception or a database error occurs
	 */
	@Override
	public void sendControlCommand(int commandID, int data) throws SQLException {
		switch (commandID) {
			case ControlCommands.AUTO_COMMIT:
				((EmbeddedProtocol)protocol).getEmbeddedConnection().sendAutocommitCommand(data);
				break;
			case ControlCommands.RELEASE:
				((EmbeddedProtocol)protocol).getEmbeddedConnection().sendReleaseCommand(data);
				break;
			case ControlCommands.CLOSE:
				((EmbeddedProtocol)protocol).getEmbeddedConnection().sendCloseCommand(data);
				break;
			//ControlCommands.REPLY_SIZE should never be sent
		}
	}

	/**
	 * Execute a batch query in an embedded connection.
	 *
	 * @param statement The original MonetStatement where the batch comes from
	 * @param batch The list of queries to execute
	 * @param counts The return of the update statement of each input query
	 * @param e An exception to be thrown if an error occurs
	 * @return If all queries in the batch executed successfully or not
	 * @throws SQLException if an IO exception or a database error occurs
	 */
	@Override
	protected boolean executeNextQueryBatch(MonetStatement statement, List<String> batch, int[] counts,
											BatchUpdateException e) throws SQLException {
		int i = 0;
		boolean error = false;
		for (String query : batch) {
			error |= statement.internalBatch(query, counts, i, i + 1, e);
			i++;
		}
		return error;
	}
}
