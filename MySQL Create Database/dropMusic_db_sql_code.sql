CREATE TABLE musica (
	musica_id		 int UNIQUE NOT NULL AUTO_INCREMENT,
	nome		 varchar(512) NOT NULL,
	genero		 varchar(512) NOT NULL,
	data_lancamento	 date NOT NULL,
	letra		 varchar(512),
	link		 varchar(512),
	artista_id_grupo int NOT NULL,
	PRIMARY KEY(musica_id)
);

CREATE TABLE album (
	album_id	 int UNIQUE NOT NULL AUTO_INCREMENT,
	nome		 varchar(512) NOT NULL,
	data_lancamento date NOT NULL,
	PRIMARY KEY(album_id)
);

CREATE TABLE periodoativo (
	data_inicio date NOT NULL,
	data_fim	 date,
    artista_artista_id int NOT NULL
);

CREATE TABLE utilizador (
	username varchar(512) UNIQUE NOT NULL,
	nome	 varchar(512) NOT NULL,
	editor	 boolean NOT NULL,
	password varchar(512) NOT NULL,
	PRIMARY KEY(username)
);

CREATE TABLE playlist (
	nome		 varchar(512) UNIQUE NOT NULL,
	private		 boolean NOT NULL,
	utilizador_username varchar(512),
	PRIMARY KEY(nome,utilizador_username)
);

CREATE TABLE concerto (
	nome	 varchar(512) NOT NULL,
	data	 date NOT NULL,
	pais	 varchar(512) NOT NULL,
	localidade varchar(512) NOT NULL,
    INDEX(data),
	PRIMARY KEY(nome,data)
);

CREATE TABLE critica (
	titulo		 varchar(512) NOT NULL,
	texto		 varchar(512) NOT NULL,
	pontuacao		 int NOT NULL,
	album_album_id	 int,
	utilizador_username varchar(512) NOT NULL,
	PRIMARY KEY(album_album_id, utilizador_username)
);

CREATE TABLE editora (
	nome varchar(512),
	PRIMARY KEY(nome)
);

CREATE TABLE partilha (
	partilha_id  int UNIQUE NOT NULL AUTO_INCREMENT,
	data		 date NOT NULL,
	utilizador_username_partilhado varchar(512) NOT NULL,
    utilizador_username_partilhou  varchar(512) NOT NULL,
	musica_musica_id	 int NOT NULL,
	PRIMARY KEY(partilha_id)
);

CREATE TABLE artista (
	artista_id		 int UNIQUE NOT NULL AUTO_INCREMENT,
	nome			 varchar(512),
	data_nascimento		 date,
	nacionalidade		 varchar(512),
	historia			 varchar(1024),
	compositor_compositor	 boolean NOT NULL,
	musico_musico		 boolean NOT NULL,
	grupomusical_grupomusical boolean NOT NULL,
	PRIMARY KEY(artista_id)
);

CREATE TABLE grupomusical_musico (
	artista_id_grupo int,
	artista_id_musico int,
	PRIMARY KEY(artista_id_grupo, artista_id_musico)
);

CREATE TABLE artista_concerto (
	artista_artista_id int,
	concerto_nome	 varchar(512),
	concerto_data	 date,
	PRIMARY KEY(artista_artista_id,concerto_nome,concerto_data)
);

CREATE TABLE compositor_artista_musica (
	artista_artista_id int,
	musica_musica_id	 int,
	PRIMARY KEY(artista_artista_id,musica_musica_id)
);

CREATE TABLE editora_grupomusical (
	editora_nome	 varchar(512) NOT NULL,
	artista_artista_id int,
	PRIMARY KEY(artista_artista_id)
);

CREATE TABLE concerto_musica (
	concerto_nome	 varchar(512),
	concerto_data	 date,
	musica_musica_id int,
	PRIMARY KEY(concerto_nome,concerto_data,musica_musica_id)
);

CREATE TABLE musica_playlist (
	musica_musica_id		 int,
	playlist_nome		 varchar(512),
	playlist_utilizador_username varchar(512),
	PRIMARY KEY(musica_musica_id,playlist_nome,playlist_utilizador_username)
);

CREATE TABLE album_musica (
	album_album_id	 int,
	musica_musica_id int,
	PRIMARY KEY(album_album_id,musica_musica_id)
);

ALTER TABLE musica ADD CONSTRAINT musica_fk1 FOREIGN KEY (artista_id_grupo) REFERENCES artista(artista_id);
ALTER TABLE periodoativo ADD CONSTRAINT periodoativo_fk1 FOREIGN KEY (artista_artista_id) REFERENCES artista(artista_id);
ALTER TABLE playlist ADD CONSTRAINT playlist_fk1 FOREIGN KEY (utilizador_username) REFERENCES utilizador(username);
ALTER TABLE critica ADD CONSTRAINT critica_fk1 FOREIGN KEY (album_album_id) REFERENCES album(album_id);
ALTER TABLE critica ADD CONSTRAINT critica_fk2 FOREIGN KEY (utilizador_username) REFERENCES utilizador(username);
ALTER TABLE critica ADD CONSTRAINT constraint_0 CHECK ((pontuacao BETWEEN 0 AND 10) AND (pontuacao < 10));
ALTER TABLE partilha ADD CONSTRAINT partilha_fk1 FOREIGN KEY (utilizador_username_partilhado) REFERENCES utilizador(username);
ALTER TABLE partilha ADD CONSTRAINT partilha_fk2 FOREIGN KEY (utilizador_username_partilhou) REFERENCES utilizador(username);
ALTER TABLE partilha ADD CONSTRAINT partilha_fk3 FOREIGN KEY (musica_musica_id) REFERENCES musica(musica_id);
ALTER TABLE artista ADD CONSTRAINT data_lt_currentDate CHECK ((data_nascimento < current_date));
ALTER TABLE grupomusical_musico ADD CONSTRAINT grupomusical_musico_fk1 FOREIGN KEY (artista_id_grupo) REFERENCES artista(artista_id);
ALTER TABLE grupomusical_musico ADD CONSTRAINT grupomusical_musico_fk2 FOREIGN KEY (artista_id_musico) REFERENCES artista(artista_id);
ALTER TABLE artista_concerto ADD CONSTRAINT artista_concerto_fk1 FOREIGN KEY (artista_artista_id) REFERENCES artista(artista_id);
ALTER TABLE artista_concerto ADD CONSTRAINT artista_concerto_fk2 FOREIGN KEY (concerto_nome) REFERENCES concerto(nome);
ALTER TABLE artista_concerto ADD CONSTRAINT artista_concerto_fk3 FOREIGN KEY (concerto_data) REFERENCES concerto(data);
ALTER TABLE compositor_artista_musica ADD CONSTRAINT artista_musica_fk1 FOREIGN KEY (artista_artista_id) REFERENCES artista(artista_id);
ALTER TABLE compositor_artista_musica ADD CONSTRAINT artista_musica_fk2 FOREIGN KEY (musica_musica_id) REFERENCES musica(musica_id);
ALTER TABLE editora_grupomusical ADD CONSTRAINT editora_grupomusical_fk1 FOREIGN KEY (editora_nome) REFERENCES editora(nome);
ALTER TABLE editora_grupomusical ADD CONSTRAINT editora_grupomusical_fk2 FOREIGN KEY (artista_artista_id) REFERENCES artista(artista_id);
ALTER TABLE concerto_musica ADD CONSTRAINT concerto_musica_fk1 FOREIGN KEY (concerto_nome) REFERENCES concerto(nome);
ALTER TABLE concerto_musica ADD CONSTRAINT concerto_musica_fk2 FOREIGN KEY (concerto_data) REFERENCES concerto(data);
ALTER TABLE concerto_musica ADD CONSTRAINT concerto_musica_fk3 FOREIGN KEY (musica_musica_id) REFERENCES musica(musica_id);
ALTER TABLE musica_playlist ADD CONSTRAINT musica_playlist_fk1 FOREIGN KEY (musica_musica_id) REFERENCES musica(musica_id);
ALTER TABLE musica_playlist ADD CONSTRAINT musica_playlist_fk2 FOREIGN KEY (playlist_nome) REFERENCES playlist(nome) ON UPDATE CASCADE;
ALTER TABLE album_musica ADD CONSTRAINT album_musica_fk1 FOREIGN KEY (album_album_id) REFERENCES album(album_id);
ALTER TABLE album_musica ADD CONSTRAINT album_musica_fk2 FOREIGN KEY (musica_musica_id) REFERENCES musica(musica_id);

